// Copyright JG

#include "Actor/Items/FP_ItemActor.h"
#include "Inventory/Items/FP_ItemFragmentTemplate.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory/Items/FP_ItemComponent.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Player/FP_PlayerController.h"
#include "UI/HUD/FP_HUD.h"
#include "UI/Widget/Items/FP_ItemLabelManager.h"

AFP_ItemActor::AFP_ItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(false);

	ItemComponent = CreateDefaultSubobject<UFP_ItemComponent>(TEXT("ItemComponent"));

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(Mesh);
	NiagaraComponent->bAutoActivate = false;
}

void AFP_ItemActor::BeginPlay()
{
	Super::BeginPlay();

	// Blueprint CDO may have SimulatePhysics=true — shut it down before the deferred enable below.
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Mesh->AddRelativeRotation(MeshRotationOffset);
	Mesh->AddRelativeLocation(MeshLocationOffset);

	ItemComponent->GetItemManifestMutable().NotifyItemSpawned();

	if (SpawnEffect)
	{
		NiagaraComponent->SetAsset(SpawnEffect);
		NiagaraComponent->Activate();
	}

	// Defer one frame so FinishSpawning fully commits the world transform before physics takes over
	FTimerHandle PhysicsTimerHandle;
	GetWorldTimerManager().SetTimer(PhysicsTimerHandle, [this]()
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		Mesh->SetSimulatePhysics(true);
		Mesh->OnComponentHit.AddDynamic(this, &ThisClass::OnMeshHit);

		const FVector RandomSpread(
			FMath::FRandRange(-LootPopSpread, LootPopSpread),
			FMath::FRandRange(-LootPopSpread, LootPopSpread),
			LootPopStrength);
		Mesh->AddImpulse(RandomSpread, NAME_None, true);

		const FVector RandomAngular(
			FMath::FRandRange(-180.f, 180.f),
			FMath::FRandRange(-180.f, 180.f),
			FMath::FRandRange(-180.f, 180.f));
		Mesh->AddAngularImpulseInDegrees(RandomAngular, NAME_None, true);

		// Fallback — force settle after 0.75s in case the item never hits anything
		GetWorldTimerManager().SetTimer(SettleTimeoutHandle, [this]()
		{
			OnMeshHit(nullptr, nullptr, nullptr, FVector::ZeroVector, FHitResult());
		}, 1.5f, false);
	}, 0.05f, false);
}

void AFP_ItemActor::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bSettled) return;
	bSettled = true;

	// Stop physics — switch to QueryOnly so character walks through but pickup raycasts still work
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->OnComponentHit.RemoveDynamic(this, &ThisClass::OnMeshHit);

	SnapToGround();
	RegisterWithLabelManager();
}

void AFP_ItemActor::SnapToGround()
{
	const FBox WorldBounds = Mesh->CalcBounds(Mesh->GetComponentTransform()).GetBox();

	// Trace from above the mesh top so we always start above ground
	const FVector Start = FVector(GetActorLocation().X, GetActorLocation().Y, WorldBounds.Max.Z + 50.f);
	const FVector End   = Start - FVector(0.f, 0.f, 650.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		// Place the pivot so the mesh's lowest bounding-box point sits on the surface
		const float PivotToBoundsBottom = GetActorLocation().Z - WorldBounds.Min.Z;
		SetActorLocation(FVector(Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z + PivotToBoundsBottom));
	}
}

void AFP_ItemActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnregisterFromLabelManager();
}

const FFP_ItemManifest& AFP_ItemActor::GetItemManifest() const
{
	return ItemComponent->GetItemManifest();
}

void AFP_ItemActor::OnPickupRequested()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC)) return;

	AFP_PlayerController* FP_PC = Cast<AFP_PlayerController>(PC);
	if (!IsValid(FP_PC)) return;

	if (!FP_PC->IsInPickupRange(this))
	{
		FP_PC->AutoRunToItem(this);
		return;
	}

	UFP_InventoryComponent* IC = UFP_AbilitySystemLibrary::GetInventoryComponent(PC);
	if (!IsValid(IC)) return;

	IC->TryAddItem(ItemComponent);
	Destroy();
}

void AFP_ItemActor::ApplyFragmentTemplate()
{
	if (!IsValid(FragmentTemplate) || !IsValid(ItemComponent)) return;

	Modify();
	ItemComponent->Modify();

	TArray<TInstancedStruct<FFP_ItemFragment>>& Existing =
		ItemComponent->GetItemManifestMutable().GetFragmentsMutable();

	for (const TInstancedStruct<FFP_ItemFragment>& TemplateFragment : FragmentTemplate->GetFragments())
	{
		const FFP_ItemFragment* FragPtr = TemplateFragment.GetPtr<FFP_ItemFragment>();
		if (!FragPtr) continue;

		const FGameplayTag Tag = FragPtr->GetFragmentTag();

		const bool bAlreadyExists = Existing.ContainsByPredicate(
			[&Tag](const TInstancedStruct<FFP_ItemFragment>& F)
			{
				const FFP_ItemFragment* Ptr = F.GetPtr<FFP_ItemFragment>();
				return Ptr && Ptr->GetFragmentTag().MatchesTagExact(Tag);
			});

		if (!bAlreadyExists)
		{
			Existing.Add(TemplateFragment);
		}
	}
}

void AFP_ItemActor::RegisterWithLabelManager()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC)) return;

	if (AFP_HUD* HUD = Cast<AFP_HUD>(PC->GetHUD()))
	{
		if (UFP_ItemLabelManager* LabelManager = HUD->GetItemLabelManager())
		{
			LabelManager->RegisterItem(this);
		}
	}
}

void AFP_ItemActor::UnregisterFromLabelManager()
{
	if (!IsValid(GetWorld())) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC)) return;

	if (AFP_HUD* HUD = Cast<AFP_HUD>(PC->GetHUD()))
	{
		if (UFP_ItemLabelManager* LabelManager = HUD->GetItemLabelManager())
		{
			LabelManager->UnregisterItem(this);
		}
	}
}

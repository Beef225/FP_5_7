// Copyright JG

#include "Actor/Items/FP_ItemActor.h"
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

	ItemComponent = CreateDefaultSubobject<UFP_ItemComponent>(TEXT("ItemComponent"));

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(Mesh);
	NiagaraComponent->bAutoActivate = false;
}

void AFP_ItemActor::BeginPlay()
{
	Super::BeginPlay();

	ItemComponent->GetItemManifestMutable().NotifyItemSpawned();

	RegisterWithLabelManager();

	if (SpawnEffect)
	{
		NiagaraComponent->SetAsset(SpawnEffect);
		NiagaraComponent->Activate();
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

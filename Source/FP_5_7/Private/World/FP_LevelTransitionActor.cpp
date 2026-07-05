// Copyright JG

#include "World/FP_LevelTransitionActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "SaveSystem/FP_SaveGameSubsystem.h"
#include "Locations/FP_LocationRegistry.h"
#include "Locations/FP_LocationDataAsset.h"
#include "UI/Widget/World/FP_InteractionPromptWidget.h"
#include "Player/FP_PlayerController.h"


AFP_LevelTransitionActor::AFP_LevelTransitionActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Set stencil value once in the constructor — highlight only needs to toggle render custom depth
	Mesh->SetCustomDepthStencilValue(252); // CUSTOM_DEPTH_TAN
	Mesh->MarkRenderStateDirty();

	MoveToComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MoveToComponent"));
	MoveToComponent->SetupAttachment(GetRootComponent());

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetupAttachment(MoveToComponent);
	TriggerSphere->SetSphereRadius(50.f);
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(GetRootComponent());
	InteractionWidget->SetWidgetSpace(EWidgetSpace::World);
	InteractionWidget->SetDrawSize(FVector2D(200.f, 80.f));
	InteractionWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f)); // reposition per-instance to sit above the mesh
	InteractionWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionWidget->SetTwoSided(true);
}

void AFP_LevelTransitionActor::BeginPlay()
{
	Super::BeginPlay();
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AFP_LevelTransitionActor::OnSphereOverlap);

	int32 DisplayLevel = 1;
	if (const UFP_SaveGameSubsystem* SaveSys = UFP_SaveGameSubsystem::Get(this))
	{
		DisplayLevel = FMath::Clamp(SaveSys->GetActiveCharacterDepth() + DepthDelta, 1, MaxAreaDepth);
	}
	const FText EffectivePromptText = FText::FromString(
		FString::Printf(TEXT("%s - LEVEL %d"), *ResolveDestinationDisplayName().ToString(), DisplayLevel));

	if (UFP_InteractionPromptWidget* PromptWidget = Cast<UFP_InteractionPromptWidget>(InteractionWidget->GetUserWidgetObject()))
	{
		PromptWidget->InitPrompt(this, EffectivePromptText);
	}

	// Always visible for now (see bOnlyShowPromptWhenHovered TODO in the header).
	if (InteractionWidget) InteractionWidget->SetVisibility(true);
}

void AFP_LevelTransitionActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	APawn* OtherPawn = Cast<APawn>(OtherActor);
	if (!OtherPawn) return;

	AFP_PlayerController* PC = Cast<AFP_PlayerController>(OtherPawn->GetController());
	if (!PC || !PC->IsPendingInteractableArrival()) return;

	PC->ConsumePendingInteractableArrival();
	TriggerTransition();
}

void AFP_LevelTransitionActor::TriggerTransition()
{
	if (!DestinationLocationTag.IsValid()) return;

	UFP_SaveGameSubsystem* SaveSys = UFP_SaveGameSubsystem::Get(this);
	if (!SaveSys) return;

	int32 NewDepth = -1; // -1 = don't touch CurrentDepth (ordinary doors, e.g. zone -> hub)
	if (bAppliesAreaDepthDelta)
	{
		NewDepth = FMath::Clamp(SaveSys->GetActiveCharacterDepth() + DepthDelta, 1, MaxAreaDepth);
	}

	SaveSys->TravelToLocation(this, DestinationLocationTag, NewDepth);
}

FText AFP_LevelTransitionActor::ResolveDestinationDisplayName() const
{
	if (const UFP_SaveGameSubsystem* SaveSys = UFP_SaveGameSubsystem::Get(this))
	{
		if (const UFP_LocationRegistry* Registry = SaveSys->GetLocationRegistry())
		{
			if (const UFP_LocationDataAsset* Location = Registry->FindLocation(DestinationLocationTag))
			{
				return Location->GetDisplayName();
			}
		}
	}

	return FText::FromName(DestinationLocationTag.GetTagName());
}

void AFP_LevelTransitionActor::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

void AFP_LevelTransitionActor::HighlightActor_Implementation()
{
	Mesh->SetRenderCustomDepth(true);

	// No-op while bOnlyShowPromptWhenHovered is false (the widget is already always
	// visible, set in BeginPlay); becomes the actual show-on-hover behavior once that
	// flag is driven by the future "hide item labels" input toggle.
	if (InteractionWidget && bOnlyShowPromptWhenHovered) InteractionWidget->SetVisibility(true);
}

void AFP_LevelTransitionActor::UnHighlightActor_Implementation()
{
	Mesh->SetRenderCustomDepth(false);
	if (InteractionWidget && bOnlyShowPromptWhenHovered) InteractionWidget->SetVisibility(false);
}

void AFP_LevelTransitionActor::Interact_Implementation(APawn* InstigatorPawn)
{
	// Called directly when the controller detects the pawn is already standing in
	// TriggerSphere at click time (so no BeginOverlap event would ever fire).
	TriggerTransition();
}

bool AFP_LevelTransitionActor::IsPawnAlreadyInRange_Implementation(APawn* InstigatorPawn)
{
	return TriggerSphere && InstigatorPawn && TriggerSphere->IsOverlappingActor(InstigatorPawn);
}

// Copyright JG

#include "World/FP_LootContainer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/FP_LootDropComponent.h"
#include "UI/Widget/World/FP_InteractionPromptWidget.h"
#include "Player/FP_PlayerController.h"

AFP_LootContainer::AFP_LootContainer()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	MoveToComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MoveToComponent"));
	MoveToComponent->SetupAttachment(GetRootComponent());

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetupAttachment(MoveToComponent);
	TriggerSphere->SetSphereRadius(50.f);
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	LootDropComponent = CreateDefaultSubobject<UFP_LootDropComponent>(TEXT("LootDropComponent"));

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(GetRootComponent());
	InteractionWidget->SetWidgetSpace(EWidgetSpace::World);
	InteractionWidget->SetDrawSize(FVector2D(200.f, 80.f));
	InteractionWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f)); // reposition per-instance to sit above the mesh
	InteractionWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionWidget->SetTwoSided(true);
}

void AFP_LootContainer::BeginPlay()
{
	Super::BeginPlay();
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AFP_LootContainer::OnSphereOverlap);

	// Stencil value is applied here (rather than only in the constructor) so it
	// covers any extra static mesh pieces added in a Blueprint subclass too — e.g.
	// a separate lid mesh on a hinge pivot — not just the C++-defined root Mesh.
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);
	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		MeshComp->SetCustomDepthStencilValue(252); // CUSTOM_DEPTH_TAN
		MeshComp->MarkRenderStateDirty();
	}

	if (UFP_InteractionPromptWidget* PromptWidget = Cast<UFP_InteractionPromptWidget>(InteractionWidget->GetUserWidgetObject()))
	{
		PromptWidget->InitPrompt(this, PromptText);
	}

	// Always visible for now (see bOnlyShowPromptWhenHovered TODO in the header).
	if (InteractionWidget) InteractionWidget->SetVisibility(true);
}

void AFP_LootContainer::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasBeenOpened) return;
	if (!OtherActor) return;

	APawn* OtherPawn = Cast<APawn>(OtherActor);
	if (!OtherPawn) return;

	AFP_PlayerController* PC = Cast<AFP_PlayerController>(OtherPawn->GetController());
	if (!PC || !PC->IsPendingInteractableArrival()) return;

	PC->ConsumePendingInteractableArrival();
	HandleArrival();
}

void AFP_LootContainer::HandleArrival()
{
	bHasBeenOpened = true;
	UnHighlightActor_Implementation();

	// Perma-hidden once opened, regardless of the always-visible default above —
	// UnHighlightActor_Implementation only hides it here because bOnlyShowPromptWhenHovered
	// is currently false, so it wouldn't otherwise touch visibility at all.
	if (InteractionWidget) InteractionWidget->SetVisibility(false);

	PlayOpenAnimation();
}

void AFP_LootContainer::PlayOpenAnimation_Implementation()
{
	// No animation asset assigned by default — spawn immediately. Override in
	// Blueprint (Timeline / Sequencer / Montage) and call OnOpenAnimationFinished
	// yourself once the animation completes.
	OnOpenAnimationFinished();
}

void AFP_LootContainer::OnOpenAnimationFinished()
{
	if (LootDropComponent)
	{
		// TODO: no area-level system exists yet — LootDropComponent::SpawnLoot only
		// sets item level when the loot spawner implements IFP_CombatInterface, which
		// this container doesn't, so drops default to level 1 (FFP_ItemFragment's
		// ItemLevel default). Once areas have a level, set it here (e.g. implement
		// IFP_CombatInterface::GetPlayerLevel_Implementation to return the area's
		// level, the same way enemies return their own level) so container loot
		// scales like enemy loot does.
		LootDropComponent->SpawnLoot();
	}
}

void AFP_LootContainer::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

void AFP_LootContainer::HighlightActor_Implementation()
{
	if (bHasBeenOpened) return;

	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);
	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		MeshComp->SetRenderCustomDepth(true);
	}

	// No-op while bOnlyShowPromptWhenHovered is false (the widget is already always
	// visible, set in BeginPlay); becomes the actual show-on-hover behavior once that
	// flag is driven by the future "hide item labels" input toggle.
	if (InteractionWidget && bOnlyShowPromptWhenHovered) InteractionWidget->SetVisibility(true);
}

void AFP_LootContainer::UnHighlightActor_Implementation()
{
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);
	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		MeshComp->SetRenderCustomDepth(false);
	}

	if (InteractionWidget && bOnlyShowPromptWhenHovered) InteractionWidget->SetVisibility(false);
}

void AFP_LootContainer::Interact_Implementation(APawn* InstigatorPawn)
{
	// Called directly when the controller detects the pawn is already standing in
	// TriggerSphere at click time (so no BeginOverlap event would ever fire).
	if (bHasBeenOpened) return;
	HandleArrival();
}

bool AFP_LootContainer::IsPawnAlreadyInRange_Implementation(APawn* InstigatorPawn)
{
	return TriggerSphere && InstigatorPawn && TriggerSphere->IsOverlappingActor(InstigatorPawn);
}

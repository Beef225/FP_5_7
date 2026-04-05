// Copyright JG

#include "World/FP_LevelTransitionActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "SaveSystem/FP_SaveGameSubsystem.h"
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
}

void AFP_LevelTransitionActor::BeginPlay()
{
	Super::BeginPlay();
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AFP_LevelTransitionActor::OnSphereOverlap);
}

void AFP_LevelTransitionActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	if (!DestinationLocationTag.IsValid()) return;

	APawn* OtherPawn = Cast<APawn>(OtherActor);
	if (!OtherPawn) return;

	AFP_PlayerController* PC = Cast<AFP_PlayerController>(OtherPawn->GetController());
	if (!PC || !PC->IsPendingLevelTransition()) return;

	PC->ConsumePendingLevelTransition();

	if (UFP_SaveGameSubsystem* SaveSys = UFP_SaveGameSubsystem::Get(this))
	{
		SaveSys->TravelToLocation(this, DestinationLocationTag);
	}
}

void AFP_LevelTransitionActor::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

void AFP_LevelTransitionActor::HighlightActor_Implementation()
{
	Mesh->SetRenderCustomDepth(true);
}

void AFP_LevelTransitionActor::UnHighlightActor_Implementation()
{
	Mesh->SetRenderCustomDepth(false);
}

void AFP_LevelTransitionActor::Interact_Implementation(APawn* InstigatorPawn)
{
	// Interaction is handled by OnSphereOverlap once the player walks to MoveToComponent.
}

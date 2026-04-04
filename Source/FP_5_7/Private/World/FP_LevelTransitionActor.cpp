// Copyright JG

#include "World/FP_LevelTransitionActor.h"
#include "Components/StaticMeshComponent.h"
#include "SaveSystem/FP_SaveGameSubsystem.h"
#include "FP_5_7.h"


AFP_LevelTransitionActor::AFP_LevelTransitionActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void AFP_LevelTransitionActor::HighlightActor_Implementation()
{
	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
}

void AFP_LevelTransitionActor::UnHighlightActor_Implementation()
{
	Mesh->SetRenderCustomDepth(false);
}

void AFP_LevelTransitionActor::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!DestinationLocationTag.IsValid()) return;

	if (UFP_SaveGameSubsystem* SaveSys = UFP_SaveGameSubsystem::Get(this))
	{
		SaveSys->TravelToLocation(this, DestinationLocationTag);
	}
}

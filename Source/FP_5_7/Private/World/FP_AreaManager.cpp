// Copyright JG

#include "World/FP_AreaManager.h"
#include "Characters/FP_EnemyCharacter.h"
#include "World/FP_LootContainer.h"
#include "SaveSystem/FP_SaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"

AFP_AreaManager::AFP_AreaManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFP_AreaManager::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimerForNextTick(this, &AFP_AreaManager::ApplyAreaLevel);
}

void AFP_AreaManager::ApplyAreaLevel()
{
	const UFP_SaveGameSubsystem* SaveSys = UFP_SaveGameSubsystem::Get(this);
	const int32 AreaLevel = SaveSys ? SaveSys->GetActiveCharacterDepth() : 1;

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFP_EnemyCharacter::StaticClass(), Enemies);
	for (AActor* Actor : Enemies)
	{
		if (AFP_EnemyCharacter* Enemy = Cast<AFP_EnemyCharacter>(Actor))
		{
			Enemy->SetEnemyLevel(AreaLevel);
		}
	}

	TArray<AActor*> Containers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFP_LootContainer::StaticClass(), Containers);
	for (AActor* Actor : Containers)
	{
		if (AFP_LootContainer* Container = Cast<AFP_LootContainer>(Actor))
		{
			Container->SetContainerItemLevel(AreaLevel);
		}
	}
}

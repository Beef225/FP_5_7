// Copyright JG

#include "Components/FP_LootDropComponent.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "Actor/Items/FP_ItemActor.h"
#include "Interaction/FP_CombatInterface.h"

UFP_LootDropComponent::UFP_LootDropComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFP_LootDropComponent::SpawnLoot()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<UFP_LootTiers*> RawExtra;
	for (TObjectPtr<UFP_LootTiers>& Tier : ExtraLootTiers)
	{
		RawExtra.Add(Tier.Get());
	}

	const TArray<FLootItem> ItemsToSpawn = UFP_AbilitySystemLibrary::GetLootItemsForEnemy(Owner, RawExtra);

	const FVector Origin = Owner->GetActorLocation();

	for (const FLootItem& Item : ItemsToSpawn)
	{
		if (!Item.LootClass) continue;

		const FVector Offset = FVector(
			FMath::RandRange(-75.f, 75.f),
			FMath::RandRange(-75.f, 75.f),
			0.f
		);

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* Spawned = Owner->GetWorld()->SpawnActor<AActor>(Item.LootClass, Origin + Offset, FRotator::ZeroRotator, Params);
		if (AFP_ItemActor* ItemActor = Cast<AFP_ItemActor>(Spawned))
		{
			if (IFP_CombatInterface* Combat = Cast<IFP_CombatInterface>(Owner))
			{
				ItemActor->SetItemLevel(Combat->GetPlayerLevel());
			}
		}
	}
}

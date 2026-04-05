// Copyright JG

#include "Components/FP_LootDropComponent.h"
#include "Libraries/FP_AbilitySystemLibrary.h"

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

		Owner->GetWorld()->SpawnActor<AActor>(Item.LootClass, Origin + Offset, FRotator::ZeroRotator, Params);
	}
}

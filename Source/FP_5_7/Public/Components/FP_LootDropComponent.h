// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Loot/Data/FP_LootTiers.h"
#include "FP_LootDropComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FP_5_7_API UFP_LootDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFP_LootDropComponent();

	/** Rolls all loot tables and spawns the results at the owner's location. */
	UFUNCTION(BlueprintCallable, Category = "Loot")
	void SpawnLoot();

	/** Extra loot tables rolled on top of the global pool when loot is triggered. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	TArray<TObjectPtr<UFP_LootTiers>> ExtraLootTiers;
};

// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FP_AreaManager.generated.h"

/**
 * Place one instance in each combat-zone level. Scales every enemy and loot
 * container found in the level to the active character's current progression
 * depth (1-85), read from AFP_PlayerController → the save system.
 *
 * Not needed in hub/safe-zone levels — leave it out there entirely, so hub loot
 * containers stay at their default item level (1) and nothing scales.
 *
 * For now this is a simple find-and-set sweep over hand-placed actors. Once
 * procedural generation exists, the spawner can assign levels directly at spawn
 * time instead, and this sweep becomes redundant (or a fallback) for anything
 * spawned after BeginPlay.
 */
UCLASS()
class FP_5_7_API AFP_AreaManager : public AActor
{
	GENERATED_BODY()

public:
	AFP_AreaManager();

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * Deferred one tick from BeginPlay so every enemy/container's own BeginPlay
	 * (which applies default level-1 state) has already completed before this
	 * overrides it — avoids racing actor initialization order within the same frame.
	 */
	void ApplyAreaLevel();
};

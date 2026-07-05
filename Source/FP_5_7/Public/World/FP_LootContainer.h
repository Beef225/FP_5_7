// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/FP_HighlightInterface.h"
#include "Interaction/FP_InteractableInterface.h"
#include "FP_LootContainer.generated.h"

class USphereComponent;
class UFP_LootDropComponent;

/**
 * A placeable world container (chest, crate, ...) that highlights in tan when hovered
 * and, once the player walks up and arrives, plays an open animation and spawns loot
 * from its LootDropComponent (global loot pool + any ExtraLootTiers assigned here) —
 * the same global/local loot-tier system enemies use.
 *
 * Usage:
 *   1. Place in the level, assign a mesh.
 *   2. Assign ExtraLootTiers on LootDropComponent for container-specific drops (on
 *      top of the global pool every container/enemy rolls).
 *   3. Position MoveToComponent in front of the container and size TriggerSphere to
 *      cover where the player should end up standing.
 *   4. If you want a visible open animation, override PlayOpenAnimation in a
 *      Blueprint subclass (Timeline / Sequencer / Anim Montage — whatever fits the
 *      mesh) and call OnOpenAnimationFinished yourself once it completes. Left
 *      unoverridden, loot spawns immediately on arrival.
 *
 * Single-use: once opened it won't re-trigger or re-highlight.
 */
UCLASS()
class FP_5_7_API AFP_LootContainer : public AActor, public IFP_HighlightInterface, public IFP_InteractableInterface
{
	GENERATED_BODY()

public:
	AFP_LootContainer();

	/** Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/** end Highlight Interface */

	/** Interactable Interface */
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	virtual bool IsPawnAlreadyInRange_Implementation(APawn* InstigatorPawn) override;
	/** end Interactable Interface */

	/**
	 * Called once the player arrives (TriggerSphere overlap). Default implementation
	 * calls OnOpenAnimationFinished immediately — override in Blueprint for a
	 * Timeline/Sequencer/Montage and call OnOpenAnimationFinished yourself once it
	 * completes.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Loot")
	void PlayOpenAnimation();

	/** Call once the open animation completes (Timeline Finished / anim notify / Delay). Spawns loot. */
	UFUNCTION(BlueprintCallable, Category="Loot")
	void OnOpenAnimationFinished();

	UFUNCTION(BlueprintPure, Category="Loot")
	bool HasBeenOpened() const { return bHasBeenOpened; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Shared by OnSphereOverlap (walked here) and Interact_Implementation (already standing here). */
	void HandleArrival();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Where the player walks to when they click this container. Position it in front of it. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> MoveToComponent;

	/** When the player enters this sphere, the container opens. Attach it to MoveToComponent. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	/** Rolls the global loot pool + ExtraLootTiers and spawns the results here once opened. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Loot")
	TObjectPtr<UFP_LootDropComponent> LootDropComponent;

	bool bHasBeenOpened = false;
};

// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Interaction/FP_HighlightInterface.h"
#include "Interaction/FP_InteractableInterface.h"
#include "FP_LevelTransitionActor.generated.h"

class USphereComponent;
class UWidgetComponent;


/**
 * A placeable actor (door, portal, exit point) that highlights in tan when hovered
 * and transports the player to a registered location when clicked.
 *
 * Usage:
 *   1. Place in the level.
 *   2. Assign a static mesh.
 *   3. Set DestinationLocationTag to the Location.X tag for the target level.
 *      The tag must be registered in your UFP_LocationRegistry data asset.
 */
UCLASS()
class FP_5_7_API AFP_LevelTransitionActor : public AActor, public IFP_HighlightInterface, public IFP_InteractableInterface
{
	GENERATED_BODY()

public:

	AFP_LevelTransitionActor();

	/** The level this actor transports the player to. Must start with "Location." */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Transition", meta=(Categories="Location"))
	FGameplayTag DestinationLocationTag;

	/**
	 * Enable for the hub's depth-relative exit doors: instead of leaving depth
	 * untouched, this door computes NewDepth = Clamp(CurrentDepth + DepthDelta, 1, MaxAreaDepth)
	 * and passes it to TravelToLocation. DestinationLocationTag stays the same
	 * static combat-zone tag on all such doors — only the depth changes, not which
	 * level loads (there's one reused zone level for now, until procedural/tileset
	 * generation exists). Leave false for ordinary doors (e.g. a zone's own door
	 * back to the hub) — depth is left untouched in that case (it only affects
	 * whether the depth is actually written on interact; the door's displayed
	 * level number below is computed the same way either way).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Transition|Depth")
	bool bAppliesAreaDepthDelta = false;

	/** -1 = one level down, 0 = same level, +1 = one level up. Also used to compute the displayed level number even when bAppliesAreaDepthDelta is false. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Transition|Depth")
	int32 DepthDelta = 0;

	/** Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/** end Highlight Interface */

	/** Interactable Interface */
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	virtual bool IsPawnAlreadyInRange_Implementation(APawn* InstigatorPawn) override;
	/** end Interactable Interface */

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Shared by OnSphereOverlap (walked here) and Interact_Implementation (already standing here). */
	void TriggerTransition();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Move the player walks to when they click this actor. Position it in front of the door. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> MoveToComponent;

	/** When the player enters this sphere the level transition fires. Attach it to MoveToComponent. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	/**
	 * World-space info/interact prompt shown while highlighted. Assign a Blueprint
	 * derived from UFP_InteractionPromptWidget as its WidgetClass in the Details
	 * panel. Reposition above the mesh per-instance as needed.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<UWidgetComponent> InteractionWidget;

	/**
	 * Resolves DestinationLocationTag through the UFP_LocationRegistry and returns
	 * its UFP_LocationDataAsset::GetDisplayName() — so the prompt's zone name always
	 * matches the destination's own display name (set once per location) rather
	 * than being retyped on every door pointing there. Falls back to the tag's own
	 * leaf name if the registry/location can't be resolved.
	 */
	FText ResolveDestinationDisplayName() const;

	/**
	 * TODO: item labels/prompts should be globally toggleable via an enhanced input
	 * action ("hide UI clutter"). When that exists, drive this from that toggle state
	 * instead of hardcoding false — false means "always visible" (current behavior,
	 * set in BeginPlay); true reactivates the existing hover-only show/hide in
	 * HighlightActor_Implementation/UnHighlightActor_Implementation below (that logic
	 * is already there, just currently a no-op while this is false).
	 */
	bool bOnlyShowPromptWhenHovered = false;

	/** Progression cap — matches the design's "up to level 85" ceiling. */
	static constexpr int32 MaxAreaDepth = 85;
};

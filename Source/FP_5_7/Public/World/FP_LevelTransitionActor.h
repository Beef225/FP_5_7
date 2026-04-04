// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Interaction/FP_HighlightInterface.h"
#include "Interaction/FP_InteractableInterface.h"
#include "FP_LevelTransitionActor.generated.h"


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

	/** Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	/** end Highlight Interface */

	/** Interactable Interface */
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	/** end Interactable Interface */

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> Mesh;
};

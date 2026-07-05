// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FP_InteractableInterface.generated.h"


UINTERFACE(MinimalAPI, BlueprintType)
class UFP_InteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by any actor the player can interact with via a short LMB click.
 * The controller calls Execute_Interact when TargetingNonEnemy + short press.
 */
class FP_5_7_API IFP_InteractableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void Interact(APawn* InstigatorPawn);

	/**
	 * Returns true if InstigatorPawn is already within this actor's arrival trigger
	 * (e.g. already standing in its TriggerSphere) at the moment it's clicked. A
	 * fresh overlap event never fires in that case, so the controller checks this
	 * before moving and calls Interact directly instead when it's already true.
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool IsPawnAlreadyInRange(APawn* InstigatorPawn);
};

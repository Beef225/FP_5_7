// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FP_PlayerAnimInstance.generated.h"

class AFP_PlayerCharacter;

/**
 * Anim instance for the player character.
 *
 * Each frame, reads the equipped weapon's left-hand IK socket and outputs
 * LeftHandIKTransform (component space) + bUseLeftHandIK for use by the
 * Two-Bone IK node in the Animation Blueprint.
 */
UCLASS()
class FP_5_7_API UFP_PlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/** Component-space position of the weapon's left-hand IK socket. Feed into Two-Bone IK effector location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IK|Left Hand")
	FVector LeftHandIKLocation = FVector::ZeroVector;

	/** True when a weapon with a valid left-hand socket is equipped. Gate the IK node with this. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IK|Left Hand")
	bool bUseLeftHandIK = false;

private:
	UPROPERTY()
	TObjectPtr<AFP_PlayerCharacter> OwningCharacter;
};

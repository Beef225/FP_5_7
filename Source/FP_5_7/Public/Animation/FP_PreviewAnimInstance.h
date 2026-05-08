// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_PreviewAnimInstance.generated.h"

class AFP_CharacterPreviewActor;
class UAnimSequence;

/**
 * Simplified anim instance for the character select / main menu preview actor.
 * No locomotion or foot IK — reads grip stance from AFP_CharacterPreviewActor
 * and resolves the matching upper-body AnimSequence from FP_UpperBodyStanceSet.
 *
 * AnimGraph should be:
 *   [Base Idle Sequence]
 *     → Layered Blend Per Bone (upper body split at spine)
 *         Blend pose: [UpperBodySequence] gated by bHasUpperBody
 *     → Output Pose
 */
UCLASS()
class FP_5_7_API UFP_PreviewAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/** Current grip style — drives which upper-body sequence is selected. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Animation")
	EWeaponGripStyle GripStance = EWeaponGripStyle::Unarmed;

	/** Upper-body stance sequence resolved from the stance set each frame. Feed into Layered Blend Per Bone. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Animation")
	TObjectPtr<UAnimSequence> UpperBodySequence;

	/** True when a valid upper-body sequence is available. Use to gate the blend node. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Animation")
	bool bHasUpperBody = false;

private:
	UPROPERTY()
	TObjectPtr<AFP_CharacterPreviewActor> OwningPreviewActor;
};

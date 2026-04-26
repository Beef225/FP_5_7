// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FP_AT_MoveSpeedRamp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFP_MoveSpeedRampDelegate);

/**
 * Ticking ability task that lerps move speed magnitude from Start to End
 * over the play length of the supplied montage, then fires OnFinished.
 *
 * Calls UFP_GA_DodgeRoll::UpdateMoveSpeedMagnitude each tick.
 */
UCLASS()
class FP_5_7_API UFP_AT_MoveSpeedRamp : public UAbilityTask
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FFP_MoveSpeedRampDelegate OnFinished;

	/**
	 * @param Montage       The montage being played — its GetPlayLength() sets the ramp duration.
	 * @param StartMagnitude Magnitude applied at t=0 (e.g. +0.5).
	 * @param EndMagnitude   Magnitude at t=Duration (e.g. -0.7).
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks",
		meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UFP_AT_MoveSpeedRamp* MoveSpeedRamp(
		UGameplayAbility* OwningAbility,
		UAnimMontage* Montage,
		float StartMagnitude,
		float EndMagnitude);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

private:

	UPROPERTY()
	TObjectPtr<UAnimMontage> TargetMontage;

	float StartMag{ 0.f };
	float EndMag{ 0.f };
	float Duration{ 1.f };
	float Elapsed{ 0.f };
};

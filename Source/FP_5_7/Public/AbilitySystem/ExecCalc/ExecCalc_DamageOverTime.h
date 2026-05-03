// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_DamageOverTime.generated.h"

/**
 * Execution calculation for damage-over-time effects.
 *
 * Differences from ExecCalc_Damage:
 *   - No dodge or block checks (DoT bypasses both defences).
 *   - No critical strike.
 *   - Applies IncreasedDamageOverTime and DamageOverTimeMultiplier on top of
 *     the standard Increased/More damage chain.
 *   - Final per-second damage is divided by DoTTickRate (default 10) so the
 *     output represents one tick of a 10 Hz periodic effect.
 *
 * Expected GE setup:
 *   - DurationPolicy  : HasDuration  (SetByCaller tag: Meta.Duration)
 *   - Period          : 0.1 (10 ticks/s)
 *   - ExecutionCalc   : this class
 *   - SetByCaller damage magnitudes same tags as ExecCalc_Damage
 */
UCLASS()
class FP_5_7_API UExecCalc_DamageOverTime : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UExecCalc_DamageOverTime();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDebugDamage = false;

	/** Number of ticks per second the periodic GE fires — must match the GE Period. */
	UPROPERTY(EditDefaultsOnly, Category="DoT", meta=(ClampMin="1.0"))
	float DoTTickRate = 10.f;
};

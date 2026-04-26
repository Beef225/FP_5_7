// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Templates/FP_GA_MovementSkill.h"
#include "Libraries/FP_EnumDefs.h"
#include "GameplayEffectTypes.h"
#include "FP_GA_DodgeRoll.generated.h"

/**
 * Dodge roll — a burst of movement in the player's input direction.
 * Bound to Spacebar (InputTag.Space), granted at game start.
 *
 * Direction detection and montage selection happen in C++.
 * The speed ramp (Blueprint Timeline +0.5 → -0.7) is driven from Blueprint
 * via UpdateMoveSpeedMagnitude each tick, backed by a SetByCaller GE.
 */
UCLASS()
class FP_5_7_API UFP_GA_DodgeRoll : public UFP_GA_MovementSkill
{
	GENERATED_BODY()

public:

	UFP_GA_DodgeRoll();

	/** Returns which direction the player is rolling based on velocity vs facing. */
	UFUNCTION(BlueprintCallable, Category="DodgeRoll")
	EFP_RollDirection GetRollDirection() const;

	// ── Move speed ramp ──────────────────────────────────────────────────────

	/** Applies the move speed GE at the given initial magnitude. Call once at ability start. */
	UFUNCTION(BlueprintCallable, Category="DodgeRoll")
	void ApplyMoveSpeedEffect(float InitialMagnitude);

	/** Updates the live GE magnitude — drive this from a Blueprint Timeline each tick. */
	UFUNCTION(BlueprintCallable, Category="DodgeRoll")
	void UpdateMoveSpeedMagnitude(float NewMagnitude);

	/** Removes the move speed GE. Call when the montage ends or ability is cancelled. */
	UFUNCTION(BlueprintCallable, Category="DodgeRoll")
	void RemoveMoveSpeedEffect();

	// ── Montages (assign in Blueprint CDO) ───────────────────────────────────

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DodgeRoll|Montages")
	TObjectPtr<UAnimMontage> Montage_Forward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DodgeRoll|Montages")
	TObjectPtr<UAnimMontage> Montage_Backward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DodgeRoll|Montages")
	TObjectPtr<UAnimMontage> Montage_Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DodgeRoll|Montages")
	TObjectPtr<UAnimMontage> Montage_Right;

	/** GE that modifies SkillMoveSpeedModifier. Must have a SetByCaller modifier
	 *  using DataTag.DodgeRoll.MoveSpeed. Assign in Blueprint CDO. */
	UPROPERTY(EditDefaultsOnly, Category="DodgeRoll|MoveSpeed")
	TSubclassOf<UGameplayEffect> MoveSpeedEffectClass;

private:

	FActiveGameplayEffectHandle MoveSpeedHandle;
};

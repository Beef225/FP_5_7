// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_GameplayAbility.h"
#include "FP_DamageGameplayAbility.generated.h"

USTRUCT(BlueprintType)
struct FDamageRange
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat DamageMin = FScalableFloat();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat DamageMax = FScalableFloat();
};



/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_DamageGameplayAbility : public UFP_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void CauseDamage(AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage|Skill")
	float GetSkillSpeedAttributeModifier() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage|Skill")
	float GetAoEAttributeModifier() const;

	/**
	 * Feed in your montage's base play rate (e.g. 2.12 to compress a 0.53s clip to 0.25s).
	 * Returns that rate scaled by the SkillSpeed attribute and freeze ramp.
	 * Use as the Rate pin on Play Montage and Wait.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Damage|Skill")
	float GetEffectiveSkillRate(float BaseRate) const;

	/**
	 * Sphere AoE. Input radius in cm; output is the effective radius after:
	 *   AdditionalRadius attribute (stored in metres, converted × 100) added to base,
	 *   then scaled by sqrt(1 + AoEMultiplier) so the impact area grows proportionally.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Damage|AoE")
	float GetEffectiveSphereRadius(float BaseRadius) const;

	/**
	 * Cone AoE projected on the ground.
	 * AdditionalRadius (metres → cm) extends length only — the cone reaches farther.
	 * AoEMultiplier scales the 2D sector area proportionally: both length and half-angle
	 * grow by (1 + AoEMultiplier)^(1/3) so the footprint is exactly that % larger.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Damage|AoE")
	void GetEffectiveConeParams(float BaseLength, float BaseHalfAngleDeg,
	                            float& OutLength, float& OutHalfAngleDeg) const;

	/**
	 * Rectangle (or square) AoE. Length is the forward extent, Width the lateral extent.
	 * AdditionalRadius (metres → cm) extends length only.
	 * AoEMultiplier scales area proportionally: both dimensions grow by sqrt(1 + AoEMultiplier).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Damage|AoE")
	void GetEffectiveRectParams(float BaseLength, float BaseWidth,
	                            float& OutLength, float& OutWidth) const;

	const TMap<FGameplayTag, FDamageRange>& GetDamageTypes() const { return DamageTypes; }


	
protected:
	void AppendSkillModifierTagsToDamageSpec(FGameplayEffectSpecHandle& DamageSpecHandle) const;
	void AssignRolledDamageMagnitudes(FGameplayEffectSpecHandle& DamageSpecHandle) const;
	void AssignSkillPassiveBonuses(FGameplayEffectSpecHandle& SpecHandle) const;

	/** Number of times to roll the damage range, keeping the highest result. Default 1. Override per skill. */
	virtual int32 GetNumDamageRolls() const { return 1; }

	/**
	 * Root tag for this skill's passive namespace (e.g. SkillPassive.LightningCoil).
	 * AssignSkillPassiveBonuses iterates PlayerState::SkillPassiveValues, matches entries
	 * whose tag starts with this prefix, and stamps them onto the damage spec as generic
	 * SetByCaller.SkillBonus.* bucket tags that the ExecCalc reads without skill-specific logic.
	 * Set this in each ability's Blueprint defaults.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Damage|Skill Passives")
	FGameplayTag SkillPassiveRootTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta=(Categories="Damage"))
	TMap<FGameplayTag, FDamageRange> DamageTypes;
};

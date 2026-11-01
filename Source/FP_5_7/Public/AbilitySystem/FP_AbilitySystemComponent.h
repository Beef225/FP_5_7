// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "FP_AbilitySystemComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitesGiven, UFP_AbilitySystemComponent* );
DECLARE_DELEGATE_OneParam(FForEachAbilty, const FGameplayAbilitySpec& );

/** One deferred hit: damage still owed and the time left to deal it. Server-only. */
USTRUCT()
struct FFP_DeferredDamageInstance
{
	GENERATED_BODY()

	float RemainingDamage = 0.f;
	float RemainingTime = 0.f;
	/** Who dealt the original hit; drip ticks use it as instigator (kill / XP credit). */
	TWeakObjectPtr<UAbilitySystemComponent> SourceASC;
};


/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:

	void AbilityActorInfoSet();
	
	FEffectAssetTags EffectAssetTags;
	FAbilitesGiven AbilitiesGivenDelegate;
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

	// Grants a skill sourced from an equipped item. No input tag is assigned here —
	// the hotbar UI assigns one when the player slots the skill.
	FGameplayAbilitySpecHandle GrantItemSkill(TSubclassOf<UFP_GameplayAbility> AbilityClass, int32 Level = 1);

	// Removes a skill previously granted by GrantItemSkill.
	void RevokeItemSkill(FGameplayAbilitySpecHandle Handle);
	bool bStartupAbilitiesGiven = false;
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	/** Adds InputTag to the target skill's spec. Does not affect other specs. */
	void AddInputTagToSkill(const FGameplayTag& SkillTag, const FGameplayTag& InputTag);

	/** Removes a specific InputTag from the target skill's spec. Does not affect other specs. */
	void RemoveInputTagFromSkill(const FGameplayTag& SkillTag, const FGameplayTag& InputTag);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);
	
	void ForEachAbility(const FForEachAbilty& Delegate);
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	// ---- Deferred damage -------------------------------------------------------------
	// Each direct hit taken while DeferredDamageAmount > 0 adds one instance (they stack).
	// Every tick each instance pays out RemainingDamage * dt / RemainingTime, so changing an
	// instance's RemainingTime re-spreads what is still owed. The owed AMOUNT never changes.
	// AUTOMATIC: whenever the DeferredDamageTime attribute changes (any GE, ability, node, gear),
	// every pending instance's remaining time is scaled by NewValue / OldValue. Dropping the
	// stat to 0 therefore pays out everything still owed on the next tick.

	/** Server only. Called from UFP_AttributeSet when ExecCalc_Damage outputs IncomingDeferredDamage. */
	void AddDeferredDamage(float Amount, float Duration, UAbilitySystemComponent* SourceASC);

	/** Set the remaining time of every pending instance (seconds). <= 0 pays everything out next tick. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "FP|Deferred Damage")
	void SetDeferredDamageRemainingTime(float NewRemainingTime);

	/** Multiply the remaining time of every pending instance (e.g. 2 = drip twice as slowly). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "FP|Deferred Damage")
	void ScaleDeferredDamageRemainingTime(float Multiplier);

	/** Total damage still owed across all instances. */
	UFUNCTION(BlueprintPure, Category = "FP|Deferred Damage")
	float GetPendingDeferredDamage() const;

	UFUNCTION(BlueprintPure, Category = "FP|Deferred Damage")
	int32 GetNumDeferredDamageInstances() const { return DeferredDamageInstances.Num(); }

	/** Drop all pending deferred damage without dealing it. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "FP|Deferred Damage")
	void ClearDeferredDamage();

	/**
	 * Shared instant GE used by UFP_GameplayAbility's native skill cost:
	 *   Heat      += SetByCaller.SkillCost.Heat
	 *   HitPoints += SetByCaller.SkillCost.HitPoints   (negative = HP paid)
	 * Built on first use (transient), so no per-skill cost GE assets are needed.
	 */
	UGameplayEffect* GetSkillCostEffect();

	/** Seconds between drip ticks. */
	UPROPERTY(EditDefaultsOnly, Category = "FP|Deferred Damage", meta = (ClampMin = "0.02"))
	float DeferredDamageTickInterval = 0.1f;


protected:

	virtual void OnRep_ActivateAbilities() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
private:
	void TickDeferredDamage();
	void OnDeferredDamageTimeChanged(const FOnAttributeChangeData& Data);
	FDelegateHandle DeferredDamageTimeChangedHandle;
	void ApplyDeferredDamageTick(float Amount, UAbilitySystemComponent* SourceASC);

	TArray<FFP_DeferredDamageInstance> DeferredDamageInstances;
	FTimerHandle DeferredDamageTimer;
	double LastDeferredTickTime = 0.0;

	/** Transient instant GE: IncomingDamage += SetByCaller.DeferredDamagePerTick. Built on first use. */
	UPROPERTY(Transient)
	TObjectPtr<UGameplayEffect> DeferredDamageTickEffect;

	UPROPERTY(Transient)
	TObjectPtr<UGameplayEffect> SkillCostEffect;
};

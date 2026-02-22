// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Interaction/FP_CombatInterface.h"
#include "GameplayEffectTypes.h"
#include "FP_CharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UAnimMontage;
class USoundBase;
struct FOnAttributeChangeData;


/**
 * Base character for FP (player + enemies).
 * Owns common GAS initialization hooks and shared combat sockets.
 */
UCLASS(Abstract)
class FP_5_7_API AFP_CharacterBase : public ACharacter, public IAbilitySystemInterface, public IFP_CombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFP_CharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	
	virtual UAnimMontage* GetDeathMontage_Implementation() override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual USoundBase* GetHitReactSound_Implementation() override;
	virtual USoundBase* GetDeathSound_Implementation() override;

	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayDeathMontage(UAnimMontage* Montage);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayDeathSound();

	
	virtual void Die() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

protected:
	virtual void BeginPlay() override;

	/**
	 * Child classes are responsible for setting:
	 * - AbilitySystemComponent
	 * - AttributeSet
	 * and calling GAS InitAbilityActorInfo(...) on their ASC.
	 *
	 * After those are valid, they should call Super::InitAbilityActorInfo()
	 * or explicitly call:
	 *  - InitializeDefaultAttributes()
	 *  - AddCharacterAbilities()
	 *  - BindMovementSpeedCallbacks()
	 *  - RefreshMovementSpeed()
	 */
	virtual void InitAbilityActorInfo();

	/** Combat */
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailTipSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName MouthSocketName;

	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	/** GAS */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	/** Default Attributes */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> PrimaryAttributeDerivedBonuses;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();

	/** Movement speed plumbing */
	void BindMovementSpeedCallbacks();
	void RefreshMovementSpeed();

	/** Attribute-change delegate signature */
	void OnMoveSpeedAttributeChanged(const FOnAttributeChangeData& Data);

	/** Tag-event delegate signature */
	UFUNCTION()
	void OnSkillMoveSpeedTagChanged(FGameplayTag Tag, int32 NewCount);

	/** Baseline MaxWalkSpeed before any attribute scaling (captured once). */
	UPROPERTY(Transient)
	float CachedBaseWalkSpeed = 0.f;

	/** Whether we already captured the base speed. */
	UPROPERTY(Transient)
	bool bCachedBaseWalkSpeed = false;

	float GetDeathRagdollDelay() const;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<USoundBase> DeathSound;


	UPROPERTY(EditAnywhere, Category="Combat")
	float DeathMontagePlayRate = 1.f;

	UPROPERTY(Transient)
	bool bDead = false;
	
	/* Dissolve Effects */

	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USoundBase> HitReactSound;

	
};

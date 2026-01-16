// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "FP_AttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

// typedef is specific to the FGameplayAttribute() signature, but TStaticFunPtr is generic to any signature chosen
//typedef TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr;
template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UFP_AttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;
	
	
	//Vital Attributes
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Vital Attributes")
	FGameplayAttributeData HitPoints;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Vital Attributes")
	FGameplayAttributeData MaxHitPoints;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Vital Attributes")
	FGameplayAttributeData Heat;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Vital Attributes")
	FGameplayAttributeData MaxHeatThreshold;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Vital Attributes")
	FGameplayAttributeData MinHeatThreshold;


//Primary Attributes
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Primary Attributes")
	FGameplayAttributeData Might;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Primary Attributes")
	FGameplayAttributeData Resonance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Primary Attributes")
	FGameplayAttributeData Agility;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Primary Attributes")
	FGameplayAttributeData Fortitude;


//Secondary Attributes
//Damage
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData AddedPhysicalDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData IncreasedPhysicalDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData MorePhysicalDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData AddedExplosiveDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData IncreasedExplosiveDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData MoreExplosiveDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData AddedRadiationDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData IncreasedRadiationDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData MoreRadiationDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData AddedChemicalDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData IncreasedChemicalDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData MoreChemicalDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData AddedEnergyDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData IncreasedEnergyDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData MoreEnergyDamage;

//Resistance
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData PhysicalDamageResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData PhysicalMaxResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ExplosiveDamageResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ExplosiveMaxResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData RadiationDamageResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData RadiationMaxResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ChemicalDamageResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ChemicalMaxResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData EnergyDamageResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData EnergyMaxResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData OverheatDamageResistance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData OverheatMaxResistance;

//Critical
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalStrikeChance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData IncreasedCriticalStrikeChance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalStrikeMultiplier;

//Defence
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData Armour;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ChanceToDodge;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData BlockChance;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData BlockedDamage;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData DeferredDamageAmount;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData DeferredDamageTime;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData HealthRegeneration;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance;

//Heat
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData AmbientTemperature;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ThermalMass;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData CoolingCoefficient;

//Debuff
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData BruisedEffectiveness;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ConcussedEffectiveness;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData IrradiatedEffectiveness;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData CorrodedEffectiveness;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ThermalDestabalizationEffectiveness;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData BruisedThreshold;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ConcussedThreshold;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData IrradiatedThreshold;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData CorrodedThreshold;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData BruisedDuration;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ConcussedDuration;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData IrradiatedDuration;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData CorrodedDuration;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ThermalInsulation;

//AoE
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData AreaOfEffectMulitplier;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData AreaOfEffectAdditionalRadius;

//Speed
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData MovementSpeed;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData SkillSpeed;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData SkillMoveSpeedModifier;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ProjectileSpeed;

//Item
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ItemQuantity;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ItemRarity;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData WattsQuantity;

//DoT
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData IncreasedDamageOverTime;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData DamageOverTimeMultiplier;

//Penetration
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ArmourPenetration;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData PhysicalResistancePenetration;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ExplosiveResistancePenetration;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData RadiationResistancePenetration;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData ChemicalResistancePenetration;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitPoints, Category = "Secondary Attributes")
	FGameplayAttributeData EnergyResistancePenetration;


//Meta Attributes
	
	//Vital Attributes
UFUNCTION()
	void OnRep_HitPoints(const FGameplayAttributeData& OldHitPoints) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, HitPoints)

UFUNCTION()
	void OnRep_MaxHitPoints(const FGameplayAttributeData& OldMaxHitPoints) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, MaxHitPoints)

UFUNCTION()
	void OnRep_Heat(const FGameplayAttributeData& OldHeat) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, Heat)

UFUNCTION()
	void OnRep_MaxHeatThreshold(const FGameplayAttributeData& OldMaxHeatThreshold) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, MaxHeatThreshold)

UFUNCTION()
	void OnRep_MinHeatThreshold(const FGameplayAttributeData& OldMinHeatThreshold) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, MinHeatThreshold)


//Primary Attributes
UFUNCTION()
	void OnRep_Might(const FGameplayAttributeData& OldMight) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, Might)

UFUNCTION()
	void OnRep_Resonance(const FGameplayAttributeData& OldResonance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, Resonance)

UFUNCTION()
	void OnRep_Agility(const FGameplayAttributeData& OldAgility) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, Agility)

UFUNCTION()
	void OnRep_Fortitude(const FGameplayAttributeData& OldFortitude) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, Fortitude)


//Secondary Attributes
//Damage
UFUNCTION()
	void OnRep_AddedPhysicalDamage(const FGameplayAttributeData& OldAddedPhysicalDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, AddedPhysicalDamage)

UFUNCTION()
	void OnRep_IncreasedPhysicalDamage(const FGameplayAttributeData& OldIncreasedPhysicalDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, IncreasedPhysicalDamage)

UFUNCTION()
	void OnRep_MorePhysicalDamage(const FGameplayAttributeData& OldMorePhysicalDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, MorePhysicalDamage)

UFUNCTION()
	void OnRep_AddedExplosiveDamage(const FGameplayAttributeData& OldAddedExplosiveDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, AddedExplosiveDamage)

UFUNCTION()
	void OnRep_IncreasedExplosiveDamage(const FGameplayAttributeData& OldIncreasedExplosiveDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, IncreasedExplosiveDamage)

UFUNCTION()
	void OnRep_MoreExplosiveDamage(const FGameplayAttributeData& OldMoreExplosiveDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, MoreExplosiveDamage)

UFUNCTION()
	void OnRep_AddedRadiationDamage(const FGameplayAttributeData& OldAddedRadiationDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, AddedRadiationDamage)

UFUNCTION()
	void OnRep_IncreasedRadiationDamage(const FGameplayAttributeData& OldIncreasedRadiationDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, IncreasedRadiationDamage)

UFUNCTION()
	void OnRep_MoreRadiationDamage(const FGameplayAttributeData& OldMoreRadiationDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, MoreRadiationDamage)

UFUNCTION()
	void OnRep_AddedChemicalDamage(const FGameplayAttributeData& OldAddedChemicalDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, AddedChemicalDamage)

UFUNCTION()
	void OnRep_IncreasedChemicalDamage(const FGameplayAttributeData& OldIncreasedChemicalDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, IncreasedChemicalDamage)

UFUNCTION()
	void OnRep_MoreChemicalDamage(const FGameplayAttributeData& OldMoreChemicalDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, MoreChemicalDamage)

UFUNCTION()
	void OnRep_AddedEnergyDamage(const FGameplayAttributeData& OldAddedEnergyDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, AddedEnergyDamage)

UFUNCTION()
	void OnRep_IncreasedEnergyDamage(const FGameplayAttributeData& OldIncreasedEnergyDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, IncreasedEnergyDamage)

UFUNCTION()
	void OnRep_MoreEnergyDamage(const FGameplayAttributeData& OldMoreEnergyDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, MoreEnergyDamage)

//Resistance
UFUNCTION()
	void OnRep_PhysicalDamageResistance(const FGameplayAttributeData& OldPhysicalDamageResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, PhysicalDamageResistance)

UFUNCTION()
	void OnRep_PhysicalMaxResistance(const FGameplayAttributeData& OldPhysicalMaxResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, PhysicalMaxResistance)

UFUNCTION()
	void OnRep_ExplosiveDamageResistance(const FGameplayAttributeData& OldExplosiveDamageResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ExplosiveDamageResistance)

UFUNCTION()
	void OnRep_ExplosiveMaxResistance(const FGameplayAttributeData& OldExplosiveMaxResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ExplosiveMaxResistance)

UFUNCTION()
	void OnRep_RadiationDamageResistance(const FGameplayAttributeData& OldRadiationDamageResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, RadiationDamageResistance)

UFUNCTION()
	void OnRep_RadiationMaxResistance(const FGameplayAttributeData& OldRadiationMaxResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, RadiationMaxResistance)

UFUNCTION()
	void OnRep_ChemicalDamageResistance(const FGameplayAttributeData& OldChemicalDamageResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ChemicalDamageResistance)

UFUNCTION()
	void OnRep_ChemicalMaxResistance(const FGameplayAttributeData& OldChemicalMaxResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ChemicalMaxResistance)

UFUNCTION()
	void OnRep_EnergyDamageResistance(const FGameplayAttributeData& OldEnergyDamageResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, EnergyDamageResistance)

UFUNCTION()
	void OnRep_EnergyMaxResistance(const FGameplayAttributeData& OldEnergyMaxResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, EnergyMaxResistance)

UFUNCTION()
	void OnRep_OverheatDamageResistance(const FGameplayAttributeData& OldOverheatDamageResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, OverheatDamageResistance)

UFUNCTION()
	void OnRep_OverheatMaxResistance(const FGameplayAttributeData& OldOverheatMaxResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, OverheatMaxResistance)

//Critical
UFUNCTION()
	void OnRep_CriticalStrikeChance(const FGameplayAttributeData& OldCriticalStrikeChance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, CriticalStrikeChance)

UFUNCTION()
	void OnRep_IncreasedCriticalStrikeChance(const FGameplayAttributeData& OldIncreasedCriticalStrikeChance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, IncreasedCriticalStrikeChance)

UFUNCTION()
	void OnRep_CriticalStrikeMultiplier(const FGameplayAttributeData& OldCriticalStrikeMultiplier) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, CriticalStrikeMultiplier)

//Defence
UFUNCTION()
	void OnRep_Armour(const FGameplayAttributeData& OldArmour) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, Armour)

UFUNCTION()
	void OnRep_ChanceToDodge(const FGameplayAttributeData& OldChanceToDodge) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ChanceToDodge)

UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, BlockChance)

UFUNCTION()
	void OnRep_BlockedDamage(const FGameplayAttributeData& OldBlockedDamage) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, BlockedDamage)

UFUNCTION()
	void OnRep_DeferredDamageAmount(const FGameplayAttributeData& OldDeferredDamageAmount) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, DeferredDamageAmount)

UFUNCTION()
	void OnRep_DeferredDamageTime(const FGameplayAttributeData& OldDeferredDamageTime) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, DeferredDamageTime)

UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, HealthRegeneration)

UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, CriticalHitResistance)

//Heat
UFUNCTION()
	void OnRep_AmbientTemperature(const FGameplayAttributeData& OldAmbientTemperature) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, AmbientTemperature)

UFUNCTION()
	void OnRep_ThermalMass(const FGameplayAttributeData& OldThermalMass) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ThermalMass)

UFUNCTION()
	void OnRep_CoolingCoefficient(const FGameplayAttributeData& OldCoolingCoefficient) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, CoolingCoefficient)

//Debuff
UFUNCTION()
	void OnRep_BruisedEffectiveness(const FGameplayAttributeData& OldBruisedEffectiveness) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, BruisedEffectiveness)

UFUNCTION()
	void OnRep_ConcussedEffectiveness(const FGameplayAttributeData& OldConcussedEffectiveness) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ConcussedEffectiveness)

UFUNCTION()
	void OnRep_IrradiatedEffectiveness(const FGameplayAttributeData& OldIrradiatedEffectiveness) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, IrradiatedEffectiveness)

UFUNCTION()
	void OnRep_CorrodedEffectiveness(const FGameplayAttributeData& OldCorrodedEffectiveness) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, CorrodedEffectiveness)

UFUNCTION()
	void OnRep_ThermalDestabalizationEffectiveness(const FGameplayAttributeData& OldThermalDestabalizationEffectiveness) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ThermalDestabalizationEffectiveness)

UFUNCTION()
	void OnRep_BruisedThreshold(const FGameplayAttributeData& OldBruisedThreshold) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, BruisedThreshold)

UFUNCTION()
	void OnRep_ConcussedThreshold(const FGameplayAttributeData& OldConcussedThreshold) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ConcussedThreshold)

UFUNCTION()
	void OnRep_IrradiatedThreshold(const FGameplayAttributeData& OldIrradiatedThreshold) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, IrradiatedThreshold)

UFUNCTION()
	void OnRep_CorrodedThreshold(const FGameplayAttributeData& OldCorrodedThreshold) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, CorrodedThreshold)

UFUNCTION()
	void OnRep_BruisedDuration(const FGameplayAttributeData& OldBruisedDuration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, BruisedDuration)

UFUNCTION()
	void OnRep_ConcussedDuration(const FGameplayAttributeData& OldConcussedDuration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ConcussedDuration)

UFUNCTION()
	void OnRep_IrradiatedDuration(const FGameplayAttributeData& OldIrradiatedDuration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, IrradiatedDuration)

UFUNCTION()
	void OnRep_CorrodedDuration(const FGameplayAttributeData& OldCorrodedDuration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, CorrodedDuration)

UFUNCTION()
	void OnRep_ThermalInsulation(const FGameplayAttributeData& OldThermalInsulation) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ThermalInsulation)

//AoE
UFUNCTION()
	void OnRep_AreaOfEffectMulitplier(const FGameplayAttributeData& OldAreaOfEffectMulitplier) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, AreaOfEffectMulitplier)

UFUNCTION()
	void OnRep_AreaOfEffectAdditionalRadius(const FGameplayAttributeData& OldAreaOfEffectAdditionalRadius) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, AreaOfEffectAdditionalRadius)

//Speed
UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, MovementSpeed)

UFUNCTION()
	void OnRep_SkillSpeed(const FGameplayAttributeData& OldSkillSpeed) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, SkillSpeed)

UFUNCTION()
	void OnRep_SkillMoveSpeedModifier(const FGameplayAttributeData& OldSkillMoveSpeedModifier) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, SkillMoveSpeedModifier)

UFUNCTION()
	void OnRep_ProjectileSpeed(const FGameplayAttributeData& OldProjectileSpeed) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ProjectileSpeed)

//Item
UFUNCTION()
	void OnRep_ItemQuantity(const FGameplayAttributeData& OldItemQuantity) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ItemQuantity)

UFUNCTION()
	void OnRep_ItemRarity(const FGameplayAttributeData& OldItemRarity) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ItemRarity)

UFUNCTION()
	void OnRep_WattsQuantity(const FGameplayAttributeData& OldWattsQuantity) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, WattsQuantity)

//DoT
UFUNCTION()
	void OnRep_IncreasedDamageOverTime(const FGameplayAttributeData& OldIncreasedDamageOverTime) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, IncreasedDamageOverTime)

UFUNCTION()
	void OnRep_DamageOverTimeMultiplier(const FGameplayAttributeData& OldDamageOverTimeMultiplier) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, DamageOverTimeMultiplier)

//Penetration
UFUNCTION()
	void OnRep_ArmourPenetration(const FGameplayAttributeData& OldArmourPenetration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ArmourPenetration)

UFUNCTION()
	void OnRep_PhysicalResistancePenetration(const FGameplayAttributeData& OldPhysicalResistancePenetration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, PhysicalResistancePenetration)

UFUNCTION()
	void OnRep_ExplosiveResistancePenetration(const FGameplayAttributeData& OldExplosiveResistancePenetration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ExplosiveResistancePenetration)

UFUNCTION()
	void OnRep_RadiationResistancePenetration(const FGameplayAttributeData& OldRadiationResistancePenetration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, RadiationResistancePenetration)

UFUNCTION()
	void OnRep_ChemicalResistancePenetration(const FGameplayAttributeData& OldChemicalResistancePenetration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, ChemicalResistancePenetration)

UFUNCTION()
	void OnRep_EnergyResistancePenetration(const FGameplayAttributeData& OldEnergyResistancePenetration) const;
	ATTRIBUTE_ACCESSORS(UFP_AttributeSet, EnergyResistancePenetration)


//Meta Attributes
	
	
private:
	void HandleIncomingDamage(const FEffectProperties& Props);
	void HandleIncomingXP(const FEffectProperties& Props);
	void Debuff(const FEffectProperties& Props);
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	void ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const;
	void SendXPEvent(const FEffectProperties& Props);
	bool bTopOffHealth = false;
	
	
};
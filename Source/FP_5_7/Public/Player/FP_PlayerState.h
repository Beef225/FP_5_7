// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_PlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UFP_LevelUpInfo;
class UFP_SkillLibrary;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPassivePointsChanged, FGameplayTag /*AttributeTag*/, int32 /*Points*/)
/**
 * 
 */
UCLASS()
class FP_5_7_API AFP_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AFP_PlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnPlayerStatChanged OnLevelChangedDelegate;
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPassivePointsChanged OnPassivePointsChangedDelegate;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetMightPassivePoints() const { return MightPassivePoints; }
	FORCEINLINE int32 GetResonancePassivePoints() const { return ResonancePassivePoints; }
	FORCEINLINE int32 GetAgilityPassivePoints() const { return AgilityPassivePoints; }
	FORCEINLINE int32 GetFortitudePassivePoints() const { return FortitudePassivePoints; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	UFP_SkillLibrary* GetSkillLibrary() const { return SkillLibrary; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	EWeaponGripStyle GetGripStance() const { return GripStance; }

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetGripStance(EWeaponGripStyle InGripStance);
	
	void AddToXP(int32 InXP);
	void AddToLevel(int32 InLevel);
	void AddToAttributePoints(int32 InPoints);
	void AddToPassivePoints(const FGameplayTag& AttributeTag, int32 InPoints);
	
	void SetXP(int32 InXP);
	void SetLevel(int32 InLevel);
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFP_LevelUpInfo> LevelUpInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UFP_SkillLibrary> SkillLibrary;

	
protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
private:

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_MightPassivePoints)
	int32 MightPassivePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_ResonancePassivePoints)
	int32 ResonancePassivePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AgilityPassivePoints)
	int32 AgilityPassivePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_FortitudePassivePoints)
	int32 FortitudePassivePoints = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);

	UFUNCTION()
	void OnRep_MightPassivePoints(int32 OldPoints);

	UFUNCTION()
	void OnRep_ResonancePassivePoints(int32 OldPoints);

	UFUNCTION()
	void OnRep_AgilityPassivePoints(int32 OldPoints);

	UFUNCTION()
	void OnRep_FortitudePassivePoints(int32 OldPoints);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_GripStance, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	EWeaponGripStyle GripStance = EWeaponGripStyle::Unarmed;
	
	UFUNCTION()
	void OnRep_GripStance(EWeaponGripStyle OldGripStance);

};

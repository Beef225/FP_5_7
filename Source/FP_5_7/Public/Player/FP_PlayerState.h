// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_PlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UFP_LevelUpInfo;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/)
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

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	EWeaponGripStyle GetGripStance() const { return GripStance; }

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetGripStance(EWeaponGripStyle InGripStance);
	
	void AddToXP(int32 InXP);
	void AddToLevel(int32 InLevel);
	
	void SetXP(int32 InXP);
	void SetLevel(int32 InLevel);
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFP_LevelUpInfo> LevelUpInfo;

	
	
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

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	
	UFUNCTION()
	void OnRep_XP(int32 OldXP);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_GripStance, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	EWeaponGripStyle GripStance = EWeaponGripStyle::Unarmed;
	
	UFUNCTION()
	void OnRep_GripStance(EWeaponGripStyle OldGripStance);

};

// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_PlayerState.generated.h"

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
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	EWeaponGripStyle GetGripStance() const { return GripStance; }

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetGripStance(EWeaponGripStyle InGripStance);

	
	
protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
private:

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_GripStance, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	EWeaponGripStyle GripStance = EWeaponGripStyle::Unarmed;
	
	UFUNCTION()
	void OnRep_GripStance(EWeaponGripStyle OldGripStance);

};

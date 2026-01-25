// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_CharacterBase.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/FP_EnemyInterface.h"
#include "UI/WidgetController/FP_OverlayWidgetController.h"
#include "FP_EnemyCharacter.generated.h"

class UWidgetComponent;

UCLASS()
class FP_5_7_API AFP_EnemyCharacter : public AFP_CharacterBase, public IFP_EnemyInterface
{
	GENERATED_BODY()

public:
	AFP_EnemyCharacter();

	/** Enemy Interface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** end Enemy Interface */

	/** Combat Interface */
	virtual int32 GetPlayerLevel() override;
	/** end Combat Interface */

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHitPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHitPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHeatChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHeatThresholdChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMinHeatThresholdChanged;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

private:
	/** Bind ASC attribute change delegates (bind BEFORE we apply default attribute effects). */
	void BindAttributeDelegates();

	/** Push initial values once (so UI is correct even if the initial set happened before binding). */
	void BroadcastInitialAttributeValues();

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HP_HeatBar;

	UPROPERTY(EditAnywhere, Category="Settings")
	bool bEnemyBarsEnabled = true;
};

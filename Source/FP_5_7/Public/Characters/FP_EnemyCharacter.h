// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_CharacterBase.h"
#include "Interaction/FP_EnemyInterface.h"
#include "FP_EnemyCharacter.generated.h"

UCLASS()
class FP_5_7_API AFP_EnemyCharacter : public AFP_CharacterBase, public IFP_EnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFP_EnemyCharacter();
	
	
	/** Enemy Interface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** end Enemy Interface */	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
};

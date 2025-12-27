// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_CharacterBase.h"
#include "FP_PlayerCharacter.generated.h"

UCLASS()
class FP_5_7_API AFP_PlayerCharacter : public AFP_CharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFP_PlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

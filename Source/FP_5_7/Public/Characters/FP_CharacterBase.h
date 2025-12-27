// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FP_CharacterBase.generated.h"

UCLASS(Abstract)
class FP_5_7_API AFP_CharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFP_CharacterBase();

protected:
	
	virtual void BeginPlay() override;

public:

};

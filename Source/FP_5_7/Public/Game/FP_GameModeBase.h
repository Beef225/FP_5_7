#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FP_GameModeBase.generated.h"

class UCharacterClassInfo;

UCLASS()
class FP_5_7_API AFP_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public: 
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
};

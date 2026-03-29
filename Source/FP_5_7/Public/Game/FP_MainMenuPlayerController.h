// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FP_MainMenuPlayerController.generated.h"


UCLASS()
class FP_5_7_API AFP_MainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};

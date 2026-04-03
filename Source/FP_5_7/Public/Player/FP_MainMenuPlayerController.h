// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FP_MainMenuPlayerController.generated.h"

class UFP_LocationRegistry;


UCLASS()
class FP_5_7_API AFP_MainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	/** Assign your location registry data asset here. */
	UPROPERTY(EditDefaultsOnly, Category="Locations")
	TObjectPtr<UFP_LocationRegistry> LocationRegistry;

protected:
	virtual void BeginPlay() override;
};

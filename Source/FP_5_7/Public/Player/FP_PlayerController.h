
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FP_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class FP_5_7_API AFP_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AFP_PlayerController();
	

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> FPContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ZoomAction;

	void Zoom(const FInputActionValue& InputActionValue);
	
	// Camera zoom
	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float MinArmLength = 350.f;

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float MaxArmLength =900.f;

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float ZoomStep = 120.f; // how much one wheel "notch" changes target

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float ZoomInterpSpeed = 12.f; // higher = snappier, lower = smoother

	float TargetArmLength = 900.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float FarPitch = -60.f;

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float NearPitch = -35.f;

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float PitchInterpSpeed = 10.f;

};


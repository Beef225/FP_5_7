// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_CharacterBase.h"
#include "FP_PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class FP_5_7_API AFP_PlayerCharacter : public AFP_CharacterBase
{
	GENERATED_BODY()

public:
	AFP_PlayerCharacter();

	// Called by PlayerController when zoom input happens
	void AddCameraZoomInput(float ZoomDelta);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	// Components
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// Zoom tuning
	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float MinArmLength = 350.f;

	// You requested max zoom out to be 800 and also the initial target length.
	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float MaxArmLength = 800.f;

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float ZoomStep = 120.f;

	// Base interp speed (we will scale it based on zoom level)
	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float ZoomInterpSpeed = 12.f;

	// Pitch tuning (optional, but included since you asked earlier)
	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float FarPitch = -60.f;   // when zoomed out

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float NearPitch = -35.f;  // when zoomed in

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float PitchInterpSpeed = 10.f;

	// Runtime state
	float TargetArmLength = 800.f;
};

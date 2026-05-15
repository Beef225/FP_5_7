// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FP_SplineVFXActor.generated.h"

class UNiagaraComponent;
class USplineComponent;

UCLASS(BlueprintType, Blueprintable)
class FP_5_7_API AFP_SplineVFXActor : public AActor
{
	GENERATED_BODY()

public:
	AFP_SplineVFXActor();

	/**
	 * Sets the spline path in world space before the effect plays.
	 * NumMidPoints intermediate points are placed at even intervals along the bolt,
	 * each randomly offset perpendicular to the bolt direction by up to MidpointOffset cm.
	 */
	UFUNCTION(BlueprintCallable, Category="SplineVFX")
	void SetEndpoints(const FVector& WorldStart, const FVector& WorldEnd,
	                  float MidpointOffset = 0.f, int32 NumMidPoints = 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SplineVFX")
	float Lifespan = 2.f;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SplineVFX")
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SplineVFX")
	TObjectPtr<UNiagaraComponent> Niagara;
};

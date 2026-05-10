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

	/** Sets the spline path in world space before the effect plays. */
	UFUNCTION(BlueprintCallable, Category="SplineVFX")
	void SetEndpoints(const FVector& WorldStart, const FVector& WorldEnd,
	                  float MidpointOffset = 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SplineVFX")
	float Lifespan = 2.f;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SplineVFX")
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SplineVFX")
	TObjectPtr<UNiagaraComponent> Niagara;
};

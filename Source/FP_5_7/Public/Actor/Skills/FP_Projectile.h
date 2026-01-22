// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FP_Projectile.generated.h"


class USphereComponent;
class UProjectileMovementComponent;
UCLASS()
class FP_5_7_API AFP_Projectile : public AActor
{
	GENERATED_BODY()

public:
	
	AFP_Projectile();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};

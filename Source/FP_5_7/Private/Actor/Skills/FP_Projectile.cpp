// Copyright JG


#include "Actor/Skills/FP_Projectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AFP_Projectile::AFP_Projectile()
{
	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->ProjectileGravityScale = 0.f;

	// Use the base defaults
	ProjectileMovement->InitialSpeed = BaseInitialSpeed;
	ProjectileMovement->MaxSpeed = BaseMaxSpeed;
}


void AFP_Projectile::InitSpeedFromDelta(float SpeedDelta)
{
	if (!ProjectileMovement) return;

	// Delta is stored as +0.3 for +30%, -0.2 for -20%, etc.
	float Scalar = 1.0f + SpeedDelta;

	// Prevent nonsensical / negative speeds. Pick a floor that makes sense for your game.
	// 0.05 means “can get very slow but not stop / reverse”.
	Scalar = FMath::Clamp(Scalar, 0.05f, 50.f);

	ProjectileMovement->InitialSpeed = BaseInitialSpeed * Scalar;
	ProjectileMovement->MaxSpeed     = BaseMaxSpeed * Scalar;

	// Preserve direction if velocity already exists.
	if (!ProjectileMovement->Velocity.IsNearlyZero())
	{
		ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * ProjectileMovement->InitialSpeed;
	}
}

void AFP_Projectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}


void AFP_Projectile::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AFP_Projectile::OnSphereOverlap);
	
}

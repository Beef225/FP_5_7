// Copyright JG

#include "Actor/Skills/FP_EjectedCasing.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "FP_5_7/FP_5_7.h"

AFP_EjectedCasing::AFP_EjectedCasing()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false; // purely cosmetic; each machine spawns its own

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Mesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->BodyInstance.bUseCCD = true; // small and fast: don't tunnel through the floor
	Mesh->CastShadow = false;
}

void AFP_EjectedCasing::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeTime);

	if (MassKg > 0.f)
	{
		Mesh->SetMassOverrideInKg(NAME_None, MassKg, true);
	}

	Mesh->OnComponentHit.AddDynamic(this, &AFP_EjectedCasing::OnMeshHit);
}

void AFP_EjectedCasing::Launch(const FVector& LinearVelocity, const FVector& AngularVelocityDeg)
{
	if (!Mesh || !Mesh->IsSimulatingPhysics()) return;

	Mesh->SetPhysicsLinearVelocity(LinearVelocity);
	Mesh->SetPhysicsAngularVelocityInDegrees(AngularVelocityDeg);
}

void AFP_EjectedCasing::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                  FVector NormalImpulse, const FHitResult& Hit)
{
	if (bPlayedBounceSound || !BounceSound) return;
	if (Mesh->GetPhysicsLinearVelocity().Size() < BounceSoundMinSpeed) return;

	bPlayedBounceSound = true;
	UGameplayStatics::PlaySoundAtLocation(this, BounceSound, Hit.ImpactPoint);
}

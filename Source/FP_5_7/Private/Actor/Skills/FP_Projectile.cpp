// Copyright JG

#include "Actor/Skills/FP_Projectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "FP_5_7/FP_5_7.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AFP_Projectile::AFP_Projectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
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

void AFP_Projectile::SetSourceActor(AActor* InSourceActor)
{
	SourceActor = InSourceActor;

	if (Sphere && SourceActor)
	{
		Sphere->IgnoreActorWhenMoving(SourceActor, true);
	}
}

void AFP_Projectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			10.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Overlap: OtherActor=%s | Instigator=%s | SourceActor=%s | Owner=%s"),
				*GetNameSafe(OtherActor),
				*GetNameSafe(GetInstigator()),
				*GetNameSafe(SourceActor),
				*GetNameSafe(GetOwner()))
		);
	}*/

	// Ignore the spawning/firing actor (preferred) and Owner (often the same)
	if (OtherActor == SourceActor || OtherActor == GetOwner())
	{
		return;
	}

	// Fallback: if someone still sets Instigator correctly, also ignore it
	if (OtherActor == GetInstigator())
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}

void AFP_Projectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);

	// Prevent overlaps/hits against the spawning actor at the collision level when possible.
	if (SourceActor)
	{
		Sphere->IgnoreActorWhenMoving(SourceActor, true);
	}

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AFP_Projectile::OnSphereOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AFP_Projectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

		if (LoopingSoundComponent)
		{
			LoopingSoundComponent->Stop();
		}
	}

	Super::Destroyed();

}

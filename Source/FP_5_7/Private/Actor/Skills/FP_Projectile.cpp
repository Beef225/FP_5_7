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
#include "Engine/World.h"
#include "CollisionQueryParams.h"

// Sets default values
AFP_Projectile::AFP_Projectile()
{
	PrimaryActorTick.bCanEverTick = true; // needed for sweep-assist
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

	PrevLocation = GetActorLocation();
}

void AFP_Projectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Only the server should authoritatively decide hits + apply effects + destroy.
	if (HasAuthority() && bEnableSweepAssist && !bHit)
	{
		Server_TrySweepAssist();
	}

	PrevLocation = GetActorLocation();
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
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	HandleImpact(OtherActor, SweepResult);
}

void AFP_Projectile::Server_TrySweepAssist()
{
	UWorld* World = GetWorld();
	if (!World || !Sphere) return;

	const FVector CurrentLocation = GetActorLocation();

	// Avoid doing work if we haven't moved meaningfully.
	if ((CurrentLocation - PrevLocation).SizeSquared() < 1.0f)
	{
		return;
	}

	const float BaseRadius = Sphere->GetScaledSphereRadius();
	const float SweepRadius = FMath::Max(1.f, BaseRadius * SweepRadiusScale);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(FP_Projectile_SweepAssist), /*bTraceComplex=*/false);
	Params.AddIgnoredActor(this);
	if (SourceActor) { Params.AddIgnoredActor(SourceActor); }
	if (GetOwner())  { Params.AddIgnoredActor(GetOwner()); }
	if (GetInstigator()) { Params.AddIgnoredActor(GetInstigator()); }

	// 1) Forward sweep: catches thin meshes / high speed tunneling.
	{
		TArray<FHitResult> Hits;
		const FCollisionShape Shape = FCollisionShape::MakeSphere(SweepRadius);

		const bool bHitSomething = World->SweepMultiByChannel(
			Hits,
			PrevLocation,
			CurrentLocation,
			FQuat::Identity,
			ECC_Pawn,     // matches your overlap response target grouping (enemy mesh commonly lives here)
			Shape,
			Params
		);

		if (bHitSomething)
		{
			for (const FHitResult& Hit : Hits)
			{
				AActor* HitActor = Hit.GetActor();
				if (!HitActor) continue;

				// Only treat as an impact if the actor can actually receive an ASC (your existing logic).
				if (UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
				{
					HandleImpact(HitActor, Hit);
					return; // impact consumed
				}
			}
		}
	}

	// 2) Downward sweep: catches “projectile is above the mesh in top-down”
	{
		const FVector DownEnd = CurrentLocation - FVector(0.f, 0.f, DownSweepDistance);

		TArray<FHitResult> Hits;
		const FCollisionShape Shape = FCollisionShape::MakeSphere(SweepRadius);

		const bool bHitSomething = World->SweepMultiByChannel(
			Hits,
			CurrentLocation,
			DownEnd,
			FQuat::Identity,
			ECC_Pawn,
			Shape,
			Params
		);

		if (bHitSomething)
		{
			for (const FHitResult& Hit : Hits)
			{
				AActor* HitActor = Hit.GetActor();
				if (!HitActor) continue;

				if (UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
				{
					HandleImpact(HitActor, Hit);
					return;
				}
			}
		}
	}
}

void AFP_Projectile::HandleImpact(AActor* HitActor, const FHitResult& Hit)
{
	if (bHit) return;
	if (!HitActor) return;

	// Ignore the spawning/firing actor (preferred) and Owner (often the same)
	if (HitActor == SourceActor || HitActor == GetOwner())
	{
		return;
	}

	// Fallback: if someone still sets Instigator correctly, also ignore it
	if (HitActor == GetInstigator())
	{
		return;
	}

	// Cosmetic (can be done server-side; you already replicate destroy + can do client-side too)
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}

	bHit = true;

	// Server applies effect + destroys
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
		{
			if (DamageEffectSpecHandle.Data.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			}
		}

		Destroy();
	}
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

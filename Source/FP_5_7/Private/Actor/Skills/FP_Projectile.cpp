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
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "AbilitySystem/Abilities/Templates/FP_ProjectileSkill.h"
#include "Interaction/FP_CombatInterface.h"
#include "Interaction/FP_EnemyInterface.h"

// Sets default values
AFP_Projectile::AFP_Projectile()
{
	PrimaryActorTick.bCanEverTick = true; // needed for sweep-assist + travel distance
	bReplicates = true;
	SetReplicatingMovement(true); // chain redirects change velocity on the server

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

	bRedirectedThisFrame = false;

	// Only the server should authoritatively decide hits + apply effects + destroy.
	if (HasAuthority() && bEnableSweepAssist && !bHit)
	{
		Server_TrySweepAssist();
	}

	const FVector CurrentLocation = GetActorLocation();
	DistanceTravelled += FVector::Dist(PrevLocation, CurrentLocation);
	PrevLocation = CurrentLocation;

	if (HasAuthority() && !bHit && MaxTravelDistance > 0.f && DistanceTravelled >= MaxTravelDistance)
	{
		StopProjectile();
	}
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

void AFP_Projectile::AddIgnoredTarget(AActor* Actor)
{
	if (Actor && !HasAlreadyHit(Actor))
	{
		HitActors.Add(Actor);
	}
}

TArray<AActor*> AFP_Projectile::GetHitActors() const
{
	TArray<AActor*> Out;
	for (const TWeakObjectPtr<AActor>& Weak : HitActors)
	{
		if (AActor* Actor = Weak.Get()) Out.Add(Actor);
	}
	return Out;
}

bool AFP_Projectile::HasAlreadyHit(const AActor* Actor) const
{
	return HitActors.ContainsByPredicate([Actor](const TWeakObjectPtr<AActor>& W) { return W.Get() == Actor; });
}

bool AFP_Projectile::IsValidTarget(const AActor* Target) const
{
	if (!Target) return false;

	const AActor* Source = SourceActor ? SourceActor.Get() : GetOwner();
	if (Target == this || Target == Source || Target == GetOwner() || Target == GetInstigator()) return false;

	if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(const_cast<AActor*>(Target))) return false;

	if (Target->Implements<UFP_CombatInterface>() && IFP_CombatInterface::Execute_IsDead(Target)) return false;

	// Players hit enemies and enemies hit players; neither side hits its own.
	if (Source && Source->Implements<UFP_EnemyInterface>() == Target->Implements<UFP_EnemyInterface>()) return false;

	return true;
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
	for (const TWeakObjectPtr<AActor>& Weak : HitActors)
	{
		if (AActor* Actor = Weak.Get()) Params.AddIgnoredActor(Actor);
	}

	const FCollisionShape Shape = FCollisionShape::MakeSphere(SweepRadius);

	// Runs one sweep and feeds every valid target to HandleImpact (several, if the projectile pierces),
	// stopping once the projectile stops or turns toward a chain target.
	auto SweepAndHit = [&](const FVector& Start, const FVector& End)
	{
		TArray<FHitResult> Hits;
		if (!World->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, Shape, Params)) return;

		for (const FHitResult& Hit : Hits)
		{
			if (bHit || bRedirectedThisFrame) return;
			AActor* HitActor = Hit.GetActor();
			if (!IsValidTarget(HitActor) || HasAlreadyHit(HitActor)) continue;
			HandleImpact(HitActor, Hit);
		}
	};

	// 1) Forward sweep: catches thin meshes / high speed tunneling.
	SweepAndHit(PrevLocation, CurrentLocation);

	// 2) Downward sweep: catches “projectile is above the mesh in top-down”
	if (!bHit && !bRedirectedThisFrame)
	{
		SweepAndHit(CurrentLocation, CurrentLocation - FVector(0.f, 0.f, DownSweepDistance));
	}
}

void AFP_Projectile::HandleImpact(AActor* HitActor, const FHitResult& Hit)
{
	if (bHit || !HitActor) return;

	const AActor* Source = SourceActor ? SourceActor.Get() : GetOwner();
	if (HitActor == Source || HitActor == GetOwner() || HitActor == GetInstigator()) return;
	if (HasAlreadyHit(HitActor)) return;

	// World geometry (anything without an ASC) stops the projectile.
	if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
	{
		PlayImpactCosmetics();
		StopProjectile();
		return;
	}

	// Dead or friendly: fly through.
	if (!IsValidTarget(HitActor)) return;

	HitActors.Add(HitActor);
	PlayImpactCosmetics();

	// Clients only play cosmetics; pierce / chain counts live on the server.
	if (!HasAuthority()) return;

	FFP_ProjectileHitInfo Info;
	Info.HitIndex     = HitCount;
	Info.PiercesSoFar = PiercesUsed;
	Info.ChainsSoFar  = ChainsUsed;
	Info.ShotIndex    = ShotIndex;
	Info.ShotGroupId  = ShotGroupId;
	Info.Generation   = Generation;
	++HitCount;

	if (UFP_ProjectileSkill* Skill = OwningSkill.Get())
	{
		Skill->HandleProjectileHit(this, HitActor, Hit, Info);
	}
	else if (DamageEffectSpecHandle.Data.IsValid())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}

	if (PierceRemaining > 0)
	{
		--PierceRemaining;
		++PiercesUsed;
		return;
	}

	if (ChainRemaining > 0 && TryRedirectToChainTarget(HitActor))
	{
		--ChainRemaining;
		++ChainsUsed;
		return;
	}

	StopProjectile();
}

bool AFP_Projectile::TryRedirectToChainTarget(const AActor* From)
{
	UWorld* World = GetWorld();
	if (!World || !From || !ProjectileMovement) return false;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(FP_Projectile_Chain), false);
	Params.AddIgnoredActor(this);
	for (const TWeakObjectPtr<AActor>& Weak : HitActors)
	{
		if (AActor* Actor = Weak.Get()) Params.AddIgnoredActor(Actor);
	}

	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByObjectType(Overlaps, From->GetActorLocation(), FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(ChainRadius), Params);

	const FVector Here = GetActorLocation();
	AActor* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Candidate = Overlap.GetActor();
		if (!IsValidTarget(Candidate) || HasAlreadyHit(Candidate)) continue;
		const float DistSq = FVector::DistSquared2D(Here, Candidate->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = Candidate;
		}
	}
	if (!Best) return false;

	// Stay at the current height (top-down: the sweep assist covers targets below).
	FVector Dir = Best->GetActorLocation() - Here;
	Dir.Z = 0.f;
	if (!Dir.Normalize()) return false;

	const float Speed = FMath::Max(ProjectileMovement->Velocity.Size(), ProjectileMovement->InitialSpeed);
	ProjectileMovement->Velocity = Dir * Speed;
	SetActorRotation(Dir.Rotation());
	bRedirectedThisFrame = true;
	return true;
}

void AFP_Projectile::PlayImpactCosmetics() const
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
}

void AFP_Projectile::StopProjectile()
{
	if (bHit) return;
	bHit = true;

	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}

	if (HasAuthority())
	{
		Destroy();
	}
}

void AFP_Projectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		PlayImpactCosmetics();

		if (LoopingSoundComponent)
		{
			LoopingSoundComponent->Stop();
		}
	}

	Super::Destroyed();
}

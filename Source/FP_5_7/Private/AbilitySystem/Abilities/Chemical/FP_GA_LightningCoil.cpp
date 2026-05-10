// Copyright JG

#include "AbilitySystem/Abilities/Chemical/FP_GA_LightningCoil.h"
#include "FP_GameplayTags.h"
#include "Interaction/FP_CombatInterface.h"
#include "Actor/Skills/FP_SplineVFXActor.h"
#include "Interaction/FP_EnemyInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/EngineTypes.h"

UFP_GA_LightningCoil::UFP_GA_LightningCoil()
{
	AbilityTags.AddTag(FFP_GameplayTags::Get().Skills_Chemical_LightningCoil);

	InstancingPolicy  = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UFP_GA_LightningCoil::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	// Blueprint takes over from here:
	//   PlayMontageAndWait → WaitGameplayEvent → FireBoltAtLocation loop → EndAbility
}

// ---------------------------------------------------------------------------
// Public BP-callable interface
// ---------------------------------------------------------------------------

FVector UFP_GA_LightningCoil::GetRandomConePoint(const FVector& Origin, const FVector& ForwardDir) const
{
	// Flatten forward to the horizontal plane so the cone spreads across the ground,
	// not up into the air or into the floor.
	FVector HorizForward = FVector(ForwardDir.X, ForwardDir.Y, 0.f);
	if (!HorizForward.Normalize()) HorizForward = FVector::ForwardVector;

	float EffLength, EffHalfAngleDeg;
	GetEffectiveConeParams(ConeLength, ConeHalfAngle, EffLength, EffHalfAngleDeg);
	const float HalfAngleRad = FMath::DegreesToRadians(EffHalfAngleDeg);

	// Random horizontal direction within the cone and random depth along it.
	const FVector RandDir   = FMath::VRandCone(HorizForward, HalfAngleRad);
	const float   Distance  = FMath::Max(
		FMath::FRandRange(EffLength * 0.2f, EffLength),
		FMath::FRandRange(EffLength * 0.2f, EffLength));

	const FVector HorizPoint = FVector(Origin.X + RandDir.X * Distance,
	                                   Origin.Y + RandDir.Y * Distance,
	                                   Origin.Z);

	// Trace straight down to snap the point to the actual ground surface.
	// Start well above the point in case the origin is below terrain level.
	const FVector TraceStart = HorizPoint + FVector(0.f, 0.f, 500.f);
	const FVector TraceEnd   = HorizPoint - FVector(0.f, 0.f, 5000.f);

	FHitResult Hit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, TraceParams))
	{
		return Hit.ImpactPoint;
	}

	return HorizPoint; // fallback if nothing is below (e.g. firing over a void)
}

void UFP_GA_LightningCoil::FireBoltAtLocation(const FVector& ImpactCenter)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !Avatar->HasAuthority()) return;

	TArray<AActor*> HitActors;
	HitActors.Add(Avatar); // always exclude caster

	ApplyBoltAoEDamage(ImpactCenter, HitActors);

	if (bCanChain && HitActors.Num() > 1)
	{
		ChainBolt(ImpactCenter, HitActors, MaxChainCount);
	}
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent implementations
// ---------------------------------------------------------------------------

int32 UFP_GA_LightningCoil::GetEffectiveNumBolts_Implementation() const
{
	// TODO: read per-skill attribute modifier when skill-tree nodes are added.
	return NumBolts;
}

// ---------------------------------------------------------------------------
// VFX
// ---------------------------------------------------------------------------

void UFP_GA_LightningCoil::SpawnBoltVFX(const FVector& Start, const FVector& End)
{
	if (!BoltActorClass) return;

	AFP_SplineVFXActor* VFX = GetWorld()->SpawnActorDeferred<AFP_SplineVFXActor>(
		BoltActorClass,
		FTransform(FRotator::ZeroRotator, Start),
		/*Owner=*/ nullptr,
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!VFX) return;

	VFX->SetEndpoints(Start, End, BoltMidpointMaxOffset);
	VFX->FinishSpawning(FTransform(FRotator::ZeroRotator, Start));
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void UFP_GA_LightningCoil::ApplyBoltAoEDamage(const FVector& ImpactCenter, TArray<AActor*>& InOutHitActors)
{
	// TODO: replace BoltAoERadius with GetAoEAttributeModifier() once SkillLibrary entry is set up.
	const float Radius = BoltAoERadius;

	FCollisionQueryParams QueryParams;
	for (AActor* Ignored : InOutHitActors)
		QueryParams.AddIgnoredActor(Ignored);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		ImpactCenter,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(Radius),
		QueryParams);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Target = Overlap.GetActor();
		if (!Target || InOutHitActors.Contains(Target)) continue;
		if (!Target->Implements<UFP_EnemyInterface>()) continue;

		InOutHitActors.Add(Target);
		CauseDamage(Target);
	}
}

void UFP_GA_LightningCoil::ChainBolt(const FVector& ImpactCenter, TArray<AActor*>& InOutHitActors, int32 RemainingChains)
{
	if (!bCanChain || RemainingChains <= 0) return;

	FCollisionQueryParams QueryParams;
	for (AActor* Ignored : InOutHitActors)
		QueryParams.AddIgnoredActor(Ignored);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		ImpactCenter,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(ChainRadius),
		QueryParams);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Target = Overlap.GetActor();
		if (!Target || InOutHitActors.Contains(Target)) continue;
		if (!Target->Implements<UFP_EnemyInterface>()) continue;

		InOutHitActors.Add(Target);
		CauseDamage(Target);
		ChainBolt(Target->GetActorLocation(), InOutHitActors, RemainingChains - 1);
		break;
	}
}

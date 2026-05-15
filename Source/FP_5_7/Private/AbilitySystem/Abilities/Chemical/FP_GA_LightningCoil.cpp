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
#include "Player/FP_PlayerState.h"

UFP_GA_LightningCoil::UFP_GA_LightningCoil()
{
	FGameplayTagContainer NewTags;
	NewTags.AddTag(FFP_GameplayTags::Get().Skills_Chemical_LightningCoil);
	SetAssetTags(NewTags);

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

	// Passive ConeLength adds a flat amount to the base before AoE attribute modifiers.
	float ExtraConeLength = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
		ExtraConeLength = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_LightningCoil_ConeLength);

	// Convergence-adjusted angle as base; parent handles AoE modifiers on top.
	float EffLength, EffHalfAngleDeg;
	GetEffectiveConeParams(ConeLength + ExtraConeLength, GetCurrentConeAngle(), EffLength, EffHalfAngleDeg);
	const float HalfAngleRad = FMath::DegreesToRadians(EffHalfAngleDeg);

	// Roll distance — base 2 rolls, keep furthest; extra rolls from DistanceLuckyRolls passive.
	int32 NumDistRolls = 2;
	if (const AFP_PlayerState* PS = GetLocalPS())
		NumDistRolls += FMath::Max(FMath::RoundToInt(PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_LightningCoil_DistanceLuckyRolls)), 0);

	const FVector RandDir = FMath::VRandCone(HorizForward, HalfAngleRad);
	float Distance = FMath::FRandRange(EffLength * 0.2f, EffLength);
	for (int32 i = 1; i < NumDistRolls; ++i)
		Distance = FMath::Max(Distance, FMath::FRandRange(EffLength * 0.2f, EffLength));

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

	// Converge the cone for the NEXT shot before applying this shot's damage.
	OnBoltFired();

	TArray<AActor*> HitActors;
	HitActors.Add(Avatar); // always exclude caster

	ApplyBoltAoEDamage(ImpactCenter, HitActors);

	// Chain count: passive stacks on top of MaxChainCount; also enables chaining when > 0.
	int32 EffectiveChainCount = MaxChainCount;
	if (const AFP_PlayerState* PS = GetLocalPS())
		EffectiveChainCount += FMath::RoundToInt(PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_LightningCoil_ChainCount));

	if ((bCanChain || EffectiveChainCount > 0) && HitActors.Num() > 1)
		ChainBolt(ImpactCenter, HitActors, EffectiveChainCount);
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent implementations
// ---------------------------------------------------------------------------

int32 UFP_GA_LightningCoil::GetEffectiveNumBolts_Implementation() const
{
	int32 Extra = 0;
	if (const APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
		if (const AFP_PlayerState* PS = Pawn->GetPlayerState<AFP_PlayerState>())
			Extra = FMath::RoundToInt(PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_LightningCoil_ExtraBolts));

	return FMath::Max(NumBolts + Extra, 1);
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

	VFX->SetEndpoints(Start, End, BoltMidpointMaxOffset, BoltMidPoints);
	VFX->FinishSpawning(FTransform(FRotator::ZeroRotator, Start));
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void UFP_GA_LightningCoil::ApplyBoltAoEDamage(const FVector& ImpactCenter, TArray<AActor*>& InOutHitActors)
{
	float ExtraRadius = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
		ExtraRadius = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_LightningCoil_BoltAoERadius);
	const float Radius = BoltAoERadius + ExtraRadius;

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

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

const AFP_PlayerState* UFP_GA_LightningCoil::GetLocalPS() const
{
	const APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	return Pawn ? Pawn->GetPlayerState<AFP_PlayerState>() : nullptr;
}

int32 UFP_GA_LightningCoil::GetNumDamageRolls() const
{
	int32 Extra = 0;
	if (const AFP_PlayerState* PS = GetLocalPS())
		Extra = FMath::Max(FMath::RoundToInt(PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_LightningCoil_DamageLuckyRolls)), 0);
	return 1 + Extra;
}

void UFP_GA_LightningCoil::CauseDamage(AActor* TargetActor)
{
	Super::CauseDamage(TargetActor);

	if (!TargetActor) return;
	const AFP_PlayerState* PS = GetLocalPS();
	if (!PS) return;

	const float Buildup = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_LightningCoil_CorrodedBuildup);
	if (Buildup <= 0.f) return;

	FGameplayEventData EventData;
	EventData.Instigator     = GetAvatarActorFromActorInfo();
	EventData.EventMagnitude = Buildup;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		TargetActor,
		FFP_GameplayTags::Get().GameplayEvent_Debuff_Buildup_Corroded,
		EventData);
}

// ---------------------------------------------------------------------------
// Convergence
// ---------------------------------------------------------------------------

float UFP_GA_LightningCoil::GetEffectiveBaseConeAngle() const
{
	float Extra = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
		Extra = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_LightningCoil_ConeAngle);
	return ConeHalfAngle + Extra;
}

float UFP_GA_LightningCoil::GetConvergenceRate() const
{
	if (const AFP_PlayerState* PS = GetLocalPS())
		return PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_LightningCoil_ConvergenceRate);
	return 0.f;
}

float UFP_GA_LightningCoil::GetCurrentConeAngle() const
{
	const float Base = GetEffectiveBaseConeAngle();
	const float Rate = GetConvergenceRate();
	if (Rate <= 0.f || CurrentConeAngle < 0.f)
		return Base;

	const float Now  = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	const float Idle = Now - LastBoltTime;

	if (Idle <= ConvergenceIdleDelaySec)
		return CurrentConeAngle;

	// Widen back to base linearly over ConvergenceWideningDurSec once idle delay expires.
	const float T = FMath::Clamp((Idle - ConvergenceIdleDelaySec) / ConvergenceWideningDurSec, 0.f, 1.f);
	return FMath::Lerp(CurrentConeAngle, Base, T);
}

void UFP_GA_LightningCoil::OnBoltFired()
{
	const float Rate = GetConvergenceRate();
	if (Rate <= 0.f) return;

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	// Start from the current effective angle (may be partially widened if the player paused).
	const float Base = GetCurrentConeAngle();
	CurrentConeAngle = FMath::Max(Base * (1.f - Rate), ConvergenceMinAngleDeg);
	LastBoltTime     = Now;
}

// ---------------------------------------------------------------------------

void UFP_GA_LightningCoil::ChainBolt(const FVector& ImpactCenter, TArray<AActor*>& InOutHitActors, int32 RemainingChains)
{
	if (RemainingChains <= 0) return;

	float ExtraRadius = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
		ExtraRadius = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_LightningCoil_ChainRadius);
	const float EffChainRadius = ChainRadius + ExtraRadius;

	FCollisionQueryParams QueryParams;
	for (AActor* Ignored : InOutHitActors)
		QueryParams.AddIgnoredActor(Ignored);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		ImpactCenter,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(EffChainRadius),
		QueryParams);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Target = Overlap.GetActor();
		if (!Target || InOutHitActors.Contains(Target)) continue;
		if (!Target->Implements<UFP_EnemyInterface>()) continue;

		InOutHitActors.Add(Target);
		CauseDamage(Target);
		SpawnBoltVFX(ImpactCenter, Target->GetActorLocation());
		ChainBolt(Target->GetActorLocation(), InOutHitActors, RemainingChains - 1);
		break;
	}
}

// Copyright JG

#include "AbilitySystem/Abilities/Physical/FP_GA_Impact.h"

#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/OverlapResult.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Pawn.h"
#include "Interaction/FP_CombatInterface.h"
#include "Interaction/FP_EnemyInterface.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "Player/FP_PlayerState.h"

UFP_GA_Impact::UFP_GA_Impact()
{
	FGameplayTagContainer NewTags;
	NewTags.AddTag(FFP_GameplayTags::Get().Skills_Physical_Impact);
	SetAssetTags(NewTags);

	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UFP_GA_Impact::ActivateAbility(
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
	//   PlayMontageAndWait → WaitGameplayEvent(Event.Montage.1) → PerformInitialImpact → EndAbility
	// (chain AoEs keep resolving after EndAbility — see class comment)
}

// ---------------------------------------------------------------------------
// Public BP-callable interface
// ---------------------------------------------------------------------------

void UFP_GA_Impact::PerformInitialImpact()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !Avatar->HasAuthority()) return;

	// Fresh context per activation — deliberately not ability-instance state, so a
	// re-triggered swing can't stomp on a previous swing's still-resolving chain.
	FImpactChainStatePtr State = MakeShared<FImpactChainState>();
	State->HitActors.Add(Avatar); // always exclude the caster

	const FVector SocketLocation = IFP_CombatInterface::Execute_GetCombatSocketLocation(
		Avatar, FFP_GameplayTags::Get().Montage_Skill_Weapon);

	ApplySphereDamageAndScheduleChain(State, SocketLocation, GetEffectiveInitialRadius(), /*DamageMultiplier=*/1.f, /*ChainDepth=*/0);
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void UFP_GA_Impact::ApplySphereDamageAndScheduleChain(const FImpactChainStatePtr& State, const FVector& Origin, float Radius, float DamageMultiplier, int32 ChainDepth)
{
	SpawnImpactEffects(Origin, Radius);

	FCollisionQueryParams QueryParams;
	for (const TWeakObjectPtr<AActor>& Ignored : State->HitActors)
		if (AActor* IgnoredActor = Ignored.Get())
			QueryParams.AddIgnoredActor(IgnoredActor);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(Radius),
		QueryParams);

	const int32 EffectiveMaxChainDepth = GetEffectiveMaxChainDepth();
	const float ChainDelay = GetEffectiveChainDelay();

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Target = Overlap.GetActor();
		if (!Target) continue;
		if (State->HitActors.ContainsByPredicate([Target](const TWeakObjectPtr<AActor>& W) { return W.Get() == Target; })) continue;
		if (!Target->Implements<UFP_EnemyInterface>()) continue;

		State->HitActors.Add(Target);
		CauseDamage(Target, DamageMultiplier);

		if (ChainDepth < EffectiveMaxChainDepth)
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(
				this, &UFP_GA_Impact::SpawnChainedImpactAt, State, TWeakObjectPtr<AActor>(Target), ChainDepth + 1);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, ChainDelay, false);
		}
	}
}

void UFP_GA_Impact::SpawnChainedImpactAt(FImpactChainStatePtr State, TWeakObjectPtr<AActor> TargetActor, int32 ChainDepth)
{
	AActor* Target = TargetActor.Get();
	if (!Target) return; // enemy died before its chain detonation fired

	ApplySphereDamageAndScheduleChain(State, Target->GetActorLocation(), GetEffectiveSecondaryRadius(),
		GetEffectiveSecondaryDamageMultiplier(), ChainDepth);
}

void UFP_GA_Impact::SpawnImpactEffects(const FVector& Location, float Radius) const
{
	if (ImpactVFX)
	{
		const float Scale = VFXReferenceRadius > 0.f ? (Radius / VFXReferenceRadius) : 1.f;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), ImpactVFX, Location, FRotator::ZeroRotator, FVector(Scale), true, true, ENCPoolMethod::AutoRelease);
	}

	if (bDrawDebugAoESpheres)
	{
		DrawDebugSphere(GetWorld(), Location, Radius, 24, FColor::Orange, false, 1.5f);
	}
}

// ---------------------------------------------------------------------------
// Effective parameter getters
// ---------------------------------------------------------------------------

float UFP_GA_Impact::GetEffectiveInitialRadius() const
{
	float ExtraRadiusMetres = 0.f;
	float AreaOfEffectDelta = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
	{
		ExtraRadiusMetres = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Impact_InitialRadius);
		AreaOfEffectDelta = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Impact_InitialAreaOfEffect);
	}

	const float ExtraRadiusCm = ExtraRadiusMetres * 100.f;
	const float AreaScale = FMath::Max(1.f + AreaOfEffectDelta, 0.0001f);

	// Circle area ∝ r² → scale r by sqrt(1+M) to grow area proportionally (same convention as GetEffectiveSphereRadius's own global AoE terms).
	return GetEffectiveSphereRadius(BaseInitialRadius + ExtraRadiusCm) * FMath::Sqrt(AreaScale);
}

float UFP_GA_Impact::GetEffectiveSecondaryRadius() const
{
	float ExtraRadiusMetres = 0.f;
	float AreaOfEffectDelta = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
	{
		ExtraRadiusMetres = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Impact_SecondaryRadius);
		AreaOfEffectDelta = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Impact_SecondaryAreaOfEffect);
	}

	const float ExtraRadiusCm = ExtraRadiusMetres * 100.f;
	const float AreaScale = FMath::Max(1.f + AreaOfEffectDelta, 0.0001f);

	return GetEffectiveSphereRadius(BaseSecondaryRadius + ExtraRadiusCm) * FMath::Sqrt(AreaScale);
}

float UFP_GA_Impact::GetEffectiveChainDelay() const
{
	bool bFound = false;
	const float DurationAttributeDelta = GetSourceAttributeValue(UFP_AttributeSet::GetSkillDurationAttribute(), bFound);

	float ChainDelayModifier = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
		ChainDelayModifier = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Impact_ChainDelayModifier);

	const float TotalDelta = (bFound ? DurationAttributeDelta : 0.f) + ChainDelayModifier;
	const float Delay = BaseChainDelay * FMath::Max(1.f + TotalDelta, 0.f);

	return FMath::Max(Delay, MinChainDelay);
}

int32 UFP_GA_Impact::GetEffectiveMaxChainDepth() const
{
	int32 Extra = 0;
	if (const AFP_PlayerState* PS = GetLocalPS())
		Extra = FMath::RoundToInt(PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Impact_MaxChainDepth));

	return FMath::Max(BaseMaxChainDepth + Extra, 0);
}

float UFP_GA_Impact::GetEffectiveSecondaryDamageMultiplier() const
{
	float Delta = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
		Delta = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Impact_SecondaryDamageMultiplier);

	return FMath::Max(1.f + Delta, 0.f);
}

const AFP_PlayerState* UFP_GA_Impact::GetLocalPS() const
{
	const APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	return Pawn ? Pawn->GetPlayerState<AFP_PlayerState>() : nullptr;
}

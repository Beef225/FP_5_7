// Copyright JG

#include "AbilitySystem/Abilities/Physical/FP_GA_Impact.h"

#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
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
}

void UFP_GA_Impact::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		for (FTimerHandle& TimerHandle : PendingChainTimers)
			TimerManager.ClearTimer(TimerHandle);
	}
	PendingChainTimers.Reset();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ---------------------------------------------------------------------------
// Public BP-callable interface
// ---------------------------------------------------------------------------

void UFP_GA_Impact::PerformInitialImpact()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !Avatar->HasAuthority()) return;

	// Defensive reset: the ability instance is InstancedPerActor and reused across
	// activations, so a rapid re-swing must not inherit the previous swing's state.
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		for (FTimerHandle& TimerHandle : PendingChainTimers)
			TimerManager.ClearTimer(TimerHandle);
	}
	PendingChainTimers.Reset();
	HitActors.Reset();
	HitActors.Add(Avatar); // always exclude the caster

	const FVector SocketLocation = IFP_CombatInterface::Execute_GetCombatSocketLocation(
		Avatar, FFP_GameplayTags::Get().Montage_Skill_Weapon);

	ApplySphereDamageAndScheduleChain(SocketLocation, GetEffectiveInitialRadius(), /*DamageMultiplier=*/1.f, /*ChainDepth=*/0);
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void UFP_GA_Impact::ApplySphereDamageAndScheduleChain(const FVector& Origin, float Radius, float DamageMultiplier, int32 ChainDepth)
{
	FCollisionQueryParams QueryParams;
	for (AActor* Ignored : HitActors)
		QueryParams.AddIgnoredActor(Ignored);

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
		if (!Target || HitActors.Contains(Target)) continue;
		if (!Target->Implements<UFP_EnemyInterface>()) continue;

		HitActors.Add(Target);
		CauseDamage(Target, DamageMultiplier);

		if (ChainDepth < EffectiveMaxChainDepth)
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(
				this, &UFP_GA_Impact::SpawnChainedImpactAt, TWeakObjectPtr<AActor>(Target), ChainDepth + 1);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, ChainDelay, false);
			PendingChainTimers.Add(TimerHandle);
		}
	}
}

void UFP_GA_Impact::SpawnChainedImpactAt(TWeakObjectPtr<AActor> TargetActor, int32 ChainDepth)
{
	AActor* Target = TargetActor.Get();
	if (!Target) return; // enemy died before its chain detonation fired

	ApplySphereDamageAndScheduleChain(Target->GetActorLocation(), GetEffectiveSecondaryRadius(),
		GetEffectiveSecondaryDamageMultiplier(), ChainDepth);
}

// ---------------------------------------------------------------------------
// Effective parameter getters
// ---------------------------------------------------------------------------

float UFP_GA_Impact::GetEffectiveInitialRadius() const
{
	float ExtraRadius = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
		ExtraRadius = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Impact_InitialRadius);

	return GetEffectiveSphereRadius(BaseInitialRadius + ExtraRadius);
}

float UFP_GA_Impact::GetEffectiveSecondaryRadius() const
{
	float ExtraRadius = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
		ExtraRadius = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Impact_SecondaryRadius);

	return GetEffectiveSphereRadius(BaseSecondaryRadius + ExtraRadius);
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

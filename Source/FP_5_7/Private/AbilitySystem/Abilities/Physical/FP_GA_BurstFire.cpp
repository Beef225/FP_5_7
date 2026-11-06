// Copyright JG

#include "AbilitySystem/Abilities/Physical/FP_GA_BurstFire.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "FP_GameplayTags.h"
#include "Actor/Skills/FP_Projectile.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/FP_CombatInterface.h"
#include "Player/FP_PlayerState.h"
#include "TimerManager.h"
#include "Engine/World.h"

UFP_GA_BurstFire::UFP_GA_BurstFire()
{
	FGameplayTagContainer NewTags;
	NewTags.AddTag(FFP_GameplayTags::Get().Skills_Physical_BurstFire);
	SetAssetTags(NewTags);

	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UFP_GA_BurstFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// Commit first, once per burst, so the BP graph (montage etc.) only runs for a paid burst.
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Verbose, TEXT("[BurstFire] CommitAbility failed (cost/cooldown) — burst not fired."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (IsActive() && bAutoStartBurst)
	{
		StartBurst();
	}
}

void UFP_GA_BurstFire::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	// Ending (or being cancelled) stops any rounds not yet fired; rounds in flight keep going.
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BurstTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ---------------------------------------------------------------------------
// Burst
// ---------------------------------------------------------------------------

void UFP_GA_BurstFire::StartBurst()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(BurstTimerHandle);
	++BurstId;
	RoundsFired     = 0;
	RoundsThisBurst = GetEffectiveRoundsPerBurst();
	BurstHitCounts.Reset();

	const AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	if (IsVolley())
	{
		// Scatter Volley: every round at once, evenly across the fan.
		const FVector Forward = Avatar->GetActorForwardVector();
		for (int32 i = 0; i < RoundsThisBurst; ++i)
		{
			const float Alpha = RoundsThisBurst > 1 ? static_cast<float>(i) / (RoundsThisBurst - 1) : 0.5f;
			const float Yaw   = FMath::Lerp(-VolleyFanHalfAngle, VolleyFanHalfAngle, Alpha);
			FireRound(i, FRotator(0.f, Yaw, 0.f).RotateVector(Forward));
		}
		RoundsFired = RoundsThisBurst;
		if (bEndAbilityWhenBurstFinishes && IsActive())
		{
			K2_EndAbility();
		}
		return;
	}

	FireNextRound();
}

void UFP_GA_BurstFire::FireNextRound()
{
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	// Aim along the current facing each round, so the player can sweep the burst.
	const float Spread = GetEffectiveSpreadHalfAngle();
	const float Yaw    = Spread > 0.f ? FMath::FRandRange(-Spread, Spread) : 0.f;
	FireRound(RoundsFired, FRotator(0.f, Yaw, 0.f).RotateVector(Avatar->GetActorForwardVector()));
	++RoundsFired;

	if (RoundsFired >= RoundsThisBurst)
	{
		if (bEndAbilityWhenBurstFinishes && IsActive())
		{
			K2_EndAbility();
		}
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(BurstTimerHandle,
		FTimerDelegate::CreateUObject(this, &UFP_GA_BurstFire::FireNextRound),
		GetEffectiveBurstInterval(), false);
}

void UFP_GA_BurstFire::FireRound(int32 RoundIndex, const FVector& Direction)
{
	// Cosmetics run wherever the ability runs (server and the predicting client).
	PlayFireFX();

	// Server only (returns null on clients); clients see the replicated projectile.
	SpawnProjectileInDirection(GetProjectileSpawnLocation(), Direction, RoundIndex, BurstId, /*Generation=*/0);
}

// ---------------------------------------------------------------------------
// Effective values
// ---------------------------------------------------------------------------

int32 UFP_GA_BurstFire::GetEffectiveRoundsPerBurst() const
{
	const int32 Extra = FMath::RoundToInt(GetPassive(FFP_GameplayTags::Get().SkillPassive_BurstFire_ExtraRounds));
	return FMath::Max(BaseRoundsPerBurst + Extra + GetAdditionalProjectileCount(), 1);
}

float UFP_GA_BurstFire::GetEffectiveBurstInterval() const
{
	const float Delta = GetPassive(FFP_GameplayTags::Get().SkillPassive_BurstFire_BurstInterval);
	float Interval = BaseBurstInterval * FMath::Max(1.f + Delta, 0.f);

	if (bIntervalScalesWithSkillSpeed)
	{
		const float Rate = GetEffectiveSkillRate(1.f);
		if (Rate > KINDA_SMALL_NUMBER) Interval /= Rate;
	}
	return FMath::Max(Interval, MinBurstInterval);
}

float UFP_GA_BurstFire::GetBurstDuration() const
{
	return IsVolley() ? 0.f : GetEffectiveBurstInterval() * FMath::Max(GetEffectiveRoundsPerBurst() - 1, 0);
}

float UFP_GA_BurstFire::GetEffectiveSpreadHalfAngle() const
{
	const float Delta = GetPassive(FFP_GameplayTags::Get().SkillPassive_BurstFire_Spread);
	return BaseSpreadHalfAngle * FMath::Max(1.f + Delta, 0.f);
}

bool UFP_GA_BurstFire::IsVolley() const
{
	return GetPassive(FFP_GameplayTags::Get().SkillPassive_BurstFire_Volley) > 0.f;
}

int32 UFP_GA_BurstFire::GetExtraPierceCount() const
{
	return FMath::RoundToInt(GetPassive(FFP_GameplayTags::Get().SkillPassive_BurstFire_PierceCount));
}

int32 UFP_GA_BurstFire::GetExtraChainCount() const
{
	return FMath::RoundToInt(GetPassive(FFP_GameplayTags::Get().SkillPassive_BurstFire_ChainCount));
}

float UFP_GA_BurstFire::GetExtraProjectileSpeedDelta() const
{
	return GetPassive(FFP_GameplayTags::Get().SkillPassive_BurstFire_ProjectileSpeed);
}

float UFP_GA_BurstFire::GetEffectiveChainRadius() const
{
	// Tree value is in metres, like the other radius passives.
	return BaseChainRadius + GetPassive(FFP_GameplayTags::Get().SkillPassive_BurstFire_ChainRadius) * 100.f;
}

// ---------------------------------------------------------------------------
// Hits
// ---------------------------------------------------------------------------

float UFP_GA_BurstFire::GetProjectileHitDamageMultiplier(const AFP_Projectile* Projectile, const AActor* Target,
                                                         const FFP_ProjectileHitInfo& Info) const
{
	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();
	float Multiplier = 1.f;

	// Pierce: lose damage per target already pierced — or, with Overpenetration, gain it.
	if (Info.PiercesSoFar > 0)
	{
		const float Overpen = GetPassive(Tags.SkillPassive_BurstFire_Overpenetration);
		const float PerPierce = Overpen > 0.f
			? 1.f + Overpen
			: FMath::Clamp(BasePierceDamageRetention + GetPassive(Tags.SkillPassive_BurstFire_PierceDamage), 0.f, 1.f);
		Multiplier *= FMath::Pow(PerPierce, static_cast<float>(Info.PiercesSoFar));
	}

	// Ricochet: lose damage per chain.
	if (Info.ChainsSoFar > 0)
	{
		const float PerChain = FMath::Clamp(
			BaseChainDamageRetention + GetPassive(Tags.SkillPassive_BurstFire_ChainDamage), 0.f, 1.f);
		Multiplier *= FMath::Pow(PerChain, static_cast<float>(Info.ChainsSoFar));
	}

	if (Info.Generation > 0)
	{
		// Fragments: a fraction of a round's damage.
		Multiplier *= FMath::Max(BaseFragmentDamage + GetPassive(Tags.SkillPassive_BurstFire_FragmentDamage), 0.f);
	}
	else if (Info.ShotGroupId == BurstId)
	{
		// Coup de Grace: the burst's last round.
		if (Info.ShotIndex == RoundsThisBurst - 1)
		{
			Multiplier *= 1.f + GetPassive(Tags.SkillPassive_BurstFire_FinalRoundDamage);
		}

		// Focus Fire: more damage for every earlier round of this burst that hit this enemy.
		const float Focus = GetPassive(Tags.SkillPassive_BurstFire_FocusFire);
		if (Focus > 0.f)
		{
			if (const int32* PriorHits = BurstHitCounts.Find(TWeakObjectPtr<AActor>(const_cast<AActor*>(Target))))
			{
				Multiplier *= 1.f + Focus * (*PriorHits);
			}
		}
	}

	return Multiplier;
}

void UFP_GA_BurstFire::OnProjectileHitApplied(AFP_Projectile* Projectile, AActor* Target, const FHitResult& Hit,
                                              const FFP_ProjectileHitInfo& Info)
{
	if (!Target) return;
	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();

	if (Info.Generation == 0 && Info.ShotGroupId == BurstId)
	{
		++BurstHitCounts.FindOrAdd(Target);
	}

	// Hollow Points: Bruised buildup per hit.
	const float Buildup = GetPassive(Tags.SkillPassive_BurstFire_BruisedBuildup);
	if (Buildup > 0.f)
	{
		FGameplayEventData EventData;
		EventData.Instigator     = GetAvatarActorFromActorInfo();
		EventData.EventMagnitude = Buildup;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, Tags.GameplayEvent_Debuff_Buildup_Bruised, EventData);
	}

	// Fragmenting Rounds: a round (not a fragment) that kills splits into fragments.
	const int32 NumFragments = FMath::RoundToInt(GetPassive(Tags.SkillPassive_BurstFire_FragmentCount));
	if (NumFragments <= 0 || Info.Generation > 0 || !Projectile) return;
	if (!Target->Implements<UFP_CombatInterface>() || !IFP_CombatInterface::Execute_IsDead(Target)) return;

	FVector Forward = Projectile->ProjectileMovement ? Projectile->ProjectileMovement->Velocity : FVector::ZeroVector;
	Forward.Z = 0.f;
	if (!Forward.Normalize()) Forward = Projectile->GetActorForwardVector();

	TArray<AActor*> Ignore = Projectile->GetHitActors();
	Ignore.AddUnique(Target);

	const FVector Origin = Projectile->GetActorLocation();
	for (int32 i = 0; i < NumFragments; ++i)
	{
		const float Alpha = NumFragments > 1 ? static_cast<float>(i) / (NumFragments - 1) : 0.5f;
		const float Yaw   = FMath::Lerp(-FragmentFanHalfAngle, FragmentFanHalfAngle, Alpha);
		if (AFP_Projectile* Fragment = SpawnProjectileIgnoring(Origin, FRotator(0.f, Yaw, 0.f).RotateVector(Forward),
			Info.ShotIndex, Info.ShotGroupId, Info.Generation + 1, Ignore))
		{
			// Fragments don't pierce or ricochet (keeps kill chains from snowballing).
			Fragment->PierceRemaining = 0;
			Fragment->ChainRemaining  = 0;
		}
	}
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

float UFP_GA_BurstFire::GetPassive(const FGameplayTag& Tag) const
{
	const AFP_PlayerState* PS = GetLocalPS();
	return PS ? PS->GetSkillPassiveValue(Tag) : 0.f;
}

const AFP_PlayerState* UFP_GA_BurstFire::GetLocalPS() const
{
	const APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	return Pawn ? Pawn->GetPlayerState<AFP_PlayerState>() : nullptr;
}

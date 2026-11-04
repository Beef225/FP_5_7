// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_DebuffPassive_Base.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "FP_GameplayTags.h"
#include "Libraries/FP_AbilitySystemLibrary.h"

void UFP_GA_DebuffPassive_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		GetBuildupEventTag(),
		nullptr,
		false,
		true
	);
	Task->EventReceived.AddDynamic(this, &UFP_GA_DebuffPassive_Base::HandleBuildupEvent);
	Task->ReadyForActivation();

	// Unstoppable only covers Bruised and Concussed.
	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();
	const FGameplayTag State = GetStateTag();
	if (State == Tags.State_Bruised || State == Tags.State_Concussed)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RegisterGameplayTagEvent(Tags.Flag_SkillTree_ImmuneToBruisedAndConcussed, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &UFP_GA_DebuffPassive_Base::OnImmunityFlagChanged);
		}
	}
}

bool UFP_GA_DebuffPassive_Base::IsImmune() const
{
	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();
	const FGameplayTag State = GetStateTag();
	if (State != Tags.State_Bruised && State != Tags.State_Concussed)
	{
		return false;
	}
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	return ASC && ASC->HasMatchingGameplayTag(Tags.Flag_SkillTree_ImmuneToBruisedAndConcussed);
}

void UFP_GA_DebuffPassive_Base::OnImmunityFlagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount <= 0)
	{
		return;
	}
	CurrentBuildup = 0.f;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DecayTimerHandle);
		if (bDebuffActive)
		{
			World->GetTimerManager().ClearTimer(DebuffDurationHandle);
		}
	}
	if (bDebuffActive)
	{
		RemoveDebuff();
	}
}

void UFP_GA_DebuffPassive_Base::SpreadDebuffsOnDeath(AActor* DyingActor)
{
	if (!DyingActor || !DyingActor->HasAuthority())
	{
		return;
	}
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DyingActor);
	if (!ASC)
	{
		return;
	}

	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();
	struct FSpread
	{
		FGameplayTag       EventTag;
		FGameplayAttribute ThresholdAttribute;
		AActor*            Source;
	};
	TArray<FSpread> Spreads;
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		for (UGameplayAbility* Instance : Spec.GetAbilityInstances())
		{
			const UFP_GA_DebuffPassive_Base* Debuff = Cast<UFP_GA_DebuffPassive_Base>(Instance);
			if (!Debuff || !Debuff->bDebuffActive || !Debuff->CachedSourceASC)
			{
				continue;
			}
			const FGameplayTag State = Debuff->GetStateTag();
			if (State != Tags.State_Irradiated && State != Tags.State_Corroded)
			{
				continue;
			}
			if (!Debuff->CachedSourceASC->HasMatchingGameplayTag(Tags.Flag_SkillTree_SpreadStatusOnKill))
			{
				continue;
			}
			if (AActor* Source = Debuff->CachedSourceASC->GetAvatarActor())
			{
				Spreads.Add({ Debuff->GetBuildupEventTag(), Debuff->GetThresholdAttribute(), Source });
			}
		}
	}
	if (Spreads.IsEmpty())
	{
		return;
	}

	TArray<AActor*> Nearby;
	UFP_AbilitySystemLibrary::GetLivePlayersWithinRadius(DyingActor, Nearby, { DyingActor },
		CascadeFailureRadius, DyingActor->GetActorLocation());

	for (const FSpread& Spread : Spreads)
	{
		for (AActor* Neighbour : Nearby)
		{
			if (!Neighbour || Neighbour == Spread.Source || !UFP_AbilitySystemLibrary::IsNotFriend(Spread.Source, Neighbour))
			{
				continue;
			}
			const UAbilitySystemComponent* NeighbourASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Neighbour);
			if (!NeighbourASC)
			{
				continue;
			}
			const float Threshold = NeighbourASC->GetSet<UFP_AttributeSet>()
				? NeighbourASC->GetNumericAttribute(Spread.ThresholdAttribute)
				: 100.f;

			FGameplayEventData EventData;
			EventData.Instigator     = Spread.Source;
			EventData.EventMagnitude = FMath::Max(Threshold, 1.f);
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Neighbour, Spread.EventTag, EventData);
		}
	}
}

void UFP_GA_DebuffPassive_Base::HandleBuildupEvent(FGameplayEventData Payload)
{
	if (bDebuffActive)
	{
		return;
	}
	if (IsImmune())
	{
		CurrentBuildup = 0.f;
		return;
	}

	// Scale incoming buildup by the post-debuff immunity ramp.
	float BuildupAmount = Payload.EventMagnitude;
	if (PostDebuffImmunityStartTime >= 0.f)
	{
		if (UWorld* World = GetWorld())
		{
			const float Elapsed = World->GetTimeSeconds() - PostDebuffImmunityStartTime;
			const float RampScale = FMath::Clamp(Elapsed / ImmunityRampDuration, 0.f, 1.f);
			BuildupAmount *= RampScale;

			if (RampScale >= 1.f)
			{
				PostDebuffImmunityStartTime = -1.f;
			}
		}
	}

	if (BuildupAmount <= 0.f)
	{
		return;
	}

	CurrentBuildup += BuildupAmount;

	// Reset the decay timer on every new buildup.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DecayTimerHandle,
			this,
			&UFP_GA_DebuffPassive_Base::OnDecayTimerExpired,
			BuildupDecayDelay,
			false
		);
	}

	// Read threshold from this character's own attribute set.
	UAbilitySystemComponent* TargetASC = GetAbilitySystemComponentFromActorInfo();
	if (!TargetASC)
	{
		return;
	}

	const bool bHasAS = TargetASC->GetSet<UFP_AttributeSet>() != nullptr;
	const float Threshold = bHasAS
		? TargetASC->GetNumericAttribute(GetThresholdAttribute())
		: 100.f;

	if (CurrentBuildup >= Threshold)
	{
		// Resolve the source ASC from the event instigator.
		UAbilitySystemComponent* SourceASC = nullptr;
		if (const AActor* Instigator = Cast<AActor>(Payload.Instigator))
		{
			if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Instigator))
			{
				SourceASC = const_cast<UAbilitySystemComponent*>(ASI->GetAbilitySystemComponent());
			}
		}

		ApplyDebuff(SourceASC);
	}
}

void UFP_GA_DebuffPassive_Base::ApplyDebuff(UAbilitySystemComponent* SourceASC)
{
	UAbilitySystemComponent* TargetASC = GetAbilitySystemComponentFromActorInfo();
	if (!TargetASC)
	{
		return;
	}

	// Stop accumulation; clear buildup and decay timer.
	bDebuffActive   = true;
	CurrentBuildup  = 0.f;
	CachedSourceASC = SourceASC;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DecayTimerHandle);
	}

	TargetASC->AddLooseGameplayTag(GetStateTag());

	// Read duration from the SOURCE's attribute set; fall back to default.
	float Duration = DefaultDuration;
	if (SourceASC)
	{
		if (const UFP_AttributeSet* SourceAS = SourceASC->GetSet<UFP_AttributeSet>())
		{
			const float SourceDuration = SourceASC->GetNumericAttribute(GetDurationAttribute());
			if (SourceDuration > 0.f)
			{
				Duration = SourceDuration;
			}
		}
	}

	OnDebuffApplied(TargetASC, SourceASC, Duration);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DebuffDurationHandle,
			this,
			&UFP_GA_DebuffPassive_Base::OnDebuffDurationExpired,
			Duration,
			false
		);
	}
}

void UFP_GA_DebuffPassive_Base::RemoveDebuff()
{
	UAbilitySystemComponent* TargetASC = GetAbilitySystemComponentFromActorInfo();
	if (TargetASC)
	{
		TargetASC->RemoveLooseGameplayTag(GetStateTag());
		OnDebuffRemoved(TargetASC);
	}

	bDebuffActive   = false;
	CachedSourceASC = nullptr;

	// Start immunity ramp: buildup will be scaled 0→100% over ImmunityRampDuration.
	if (UWorld* World = GetWorld())
	{
		PostDebuffImmunityStartTime = World->GetTimeSeconds();
	}
}

void UFP_GA_DebuffPassive_Base::ApplyDebuffEffect(UAbilitySystemComponent* TargetASC,
                                                   const TArray<TPair<FGameplayAttribute, float>>& Modifiers)
{
	if (!TargetASC || Modifiers.IsEmpty())
	{
		return;
	}

	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), NAME_None);
	Effect->DurationPolicy = EGameplayEffectDurationType::Infinite;

	for (const TPair<FGameplayAttribute, float>& Pair : Modifiers)
	{
		FGameplayModifierInfo& Mod = Effect->Modifiers.AddDefaulted_GetRef();
		Mod.Attribute        = Pair.Key;
		Mod.ModifierOp       = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Pair.Value));
	}

	const FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
	const FGameplayEffectSpec Spec(Effect, Context, 1.f);
	DebuffEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
}

void UFP_GA_DebuffPassive_Base::OnDecayTimerExpired()
{
	CurrentBuildup = 0.f;
}

void UFP_GA_DebuffPassive_Base::OnDebuffDurationExpired()
{
	RemoveDebuff();
}

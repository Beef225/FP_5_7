// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_DebuffPassive_Base.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

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
}

void UFP_GA_DebuffPassive_Base::HandleBuildupEvent(FGameplayEventData Payload)
{
	if (bDebuffActive)
	{
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

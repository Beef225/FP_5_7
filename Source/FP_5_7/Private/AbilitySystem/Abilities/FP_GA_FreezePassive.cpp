// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_FreezePassive.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"

void UFP_GA_FreezePassive::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !FreezeModifiersGEClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Apply the freeze modifier GE with an initial magnitude of 0 (no effect at start).
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(FreezeModifiersGEClass, 1.f, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FFP_GameplayTags::Get().SetByCaller_FreezeRamp, 0.f);
	FreezeGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	// Poll at the same cadence as the heat dissipation GE.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			FreezeTimerHandle,
			this,
			&UFP_GA_FreezePassive::OnFreezeTimerTick,
			0.1f,
			true
		);
	}
}

void UFP_GA_FreezePassive::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FreezeTimerHandle);
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		if (FreezeGEHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(FreezeGEHandle);
		}

		const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();
		if (bIsChilled) { ASC->RemoveReplicatedLooseGameplayTag(Tags.State_Chilled); }
		if (bIsFrozen)  { ASC->RemoveReplicatedLooseGameplayTag(Tags.State_Frozen);  }
	}

	bIsChilled = false;
	bIsFrozen  = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFP_GA_FreezePassive::OnFreezeTimerTick()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) { return; }

	const UFP_AttributeSet* AS = ASC->GetSet<UFP_AttributeSet>();
	if (!AS) { return; }

	const float Heat         = AS->GetHeat();
	const float MinThreshold = AS->GetMinHeatThreshold();

	// ChillStart is 75% of MinHeatThreshold (e.g. MinThreshold=-100 -> ChillStart=-75).
	const float ChillStart = MinThreshold * 0.75f;

	// Guard: if the two are identical (e.g. MinThreshold==0) there is no valid range.
	if (FMath::IsNearlyEqual(ChillStart, MinThreshold))
	{
		UpdateFreezeState(0.f);
		return;
	}

	// 0 when Heat >= ChillStart, ramps to 1 when Heat <= MinThreshold.
	const float FreezeRamp = FMath::Clamp(FMath::GetRangePct(ChillStart, MinThreshold, Heat), 0.f, 1.f);

	// Push the negated ramp to the GE (-0 to -1 as additive modifier on speed attributes).
	ASC->UpdateActiveGameplayEffectSetByCallerMagnitude(
		FreezeGEHandle,
		FFP_GameplayTags::Get().SetByCaller_FreezeRamp,
		-FreezeRamp
	);

	UpdateFreezeState(FreezeRamp);
}

void UFP_GA_FreezePassive::UpdateFreezeState(float FreezeRamp)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) { return; }

	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();

	const bool bShouldBeFrozen  = FreezeRamp >= 1.f;
	const bool bShouldBeChilled = FreezeRamp > 0.f && !bShouldBeFrozen;

	if (bShouldBeFrozen != bIsFrozen)
	{
		bIsFrozen = bShouldBeFrozen;
		if (bIsFrozen)
		{
			ASC->AddReplicatedLooseGameplayTag(Tags.State_Frozen);
			// Ensure Chilled is cleared when fully frozen.
			if (bIsChilled)
			{
				ASC->RemoveReplicatedLooseGameplayTag(Tags.State_Chilled);
				bIsChilled = false;
			}
		}
		else
		{
			ASC->RemoveReplicatedLooseGameplayTag(Tags.State_Frozen);
		}
	}

	if (bShouldBeChilled != bIsChilled)
	{
		bIsChilled = bShouldBeChilled;
		if (bIsChilled)
		{
			ASC->AddReplicatedLooseGameplayTag(Tags.State_Chilled);
		}
		else
		{
			ASC->RemoveReplicatedLooseGameplayTag(Tags.State_Chilled);
		}
	}
}

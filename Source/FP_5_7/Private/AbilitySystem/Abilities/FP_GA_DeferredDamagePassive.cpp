// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_DeferredDamagePassive.h"
#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"

void UFP_GA_DeferredDamagePassive::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();

	UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		Tags.GameplayEvent_DeferredDamage,
		nullptr,   // listen on self
		false,     // not one-shot — fires every time damage is deferred
		true       // exact tag match
	);
	Task->EventReceived.AddDynamic(this, &UFP_GA_DeferredDamagePassive::HandleDeferredDamageEvent);
	Task->ReadyForActivation();
}

void UFP_GA_DeferredDamagePassive::HandleDeferredDamageEvent(FGameplayEventData Payload)
{
	if (!DeferredDamageEffectClass) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	const UFP_AttributeSet* AS = ASC->GetSet<UFP_AttributeSet>();
	if (!AS) return;

	// Duration comes from the character's own DeferredDamageTime attribute —
	// same source the exec calc read when it computed the per-tick value.
	const float Duration = FMath::Max(AS->GetDeferredDamageTime(), 0.1f);

	// EventMagnitude carries the pre-computed per-tick damage (already divided
	// by tick rate and bypass defences — no exec calc needed on the GE).
	const float PerTick = Payload.EventMagnitude;
	if (PerTick <= 0.f) return;

	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DeferredDamageEffectClass, 1.f, Context);
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(Tags.SetByCaller_DeferredDamagePerTick, PerTick);
	SpecHandle.Data->SetSetByCallerMagnitude(Tags.SetByCaller_DotDuration, Duration);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

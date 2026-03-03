// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_ListenForEvents.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UFP_GA_ListenForEvents::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FFP_GameplayTags::Get().MetaAttribute_IncomingXP,
		nullptr,	// listen on own ASC
		false,		// bOnlyTriggerOnce — keep listening across all kills
		true		// bOnlyMatchExact — must be exactly MetaAttribute_IncomingXP
	);

	WaitEventTask->EventReceived.AddDynamic(this, &UFP_GA_ListenForEvents::OnXPEventReceived);
	WaitEventTask->ReadyForActivation();
}

void UFP_GA_ListenForEvents::OnXPEventReceived(FGameplayEventData Payload)
{
	if (!IncomingXPEffectClass) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(IncomingXPEffectClass, 1.f, ContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		FFP_GameplayTags::Get().MetaAttribute_IncomingXP,
		Payload.EventMagnitude);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

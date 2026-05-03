// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_ListenForEvents.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Pawn.h"
#include "Interaction/FP_CombatInterface.h"
#include "Player/FP_PlayerState.h"

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

	// Resolve monster level from the enemy actor packed into the event by SendXPEvent.
	int32 MonsterLevel = -1;
	if (const AActor* Enemy = Payload.Instigator.Get())
		if (IFP_CombatInterface* CombatInterface = const_cast<IFP_CombatInterface*>(Cast<IFP_CombatInterface>(Enemy)))
			MonsterLevel = CombatInterface->GetPlayerLevel();

	AFP_PlayerState* PS = nullptr;
	if (APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
		PS = Pawn->GetPlayerState<AFP_PlayerState>();

	// Character XP uses bidirectional penalty (penalises both farming low AND rushing high).
	const int32 RawXP = static_cast<int32>(Payload.EventMagnitude);
	const float CharMultiplier = (PS && MonsterLevel >= 0)
		? AFP_PlayerState::ComputeCharacterXPMultiplier(PS->GetPlayerLevel(), MonsterLevel)
		: 1.f;
	const float CharXP = FMath::Max(FMath::RoundToInt(static_cast<float>(RawXP) * CharMultiplier), 1);

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(IncomingXPEffectClass, 1.f, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		FFP_GameplayTags::Get().SetByCaller_Attribute_IncomingXP,
		CharXP);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	// Skill XP passes raw amount; AddSkillXP applies its own one-directional penalty
	// so an overlevelled character with a new low-level skill still earns full skill XP.
	if (PS)
		PS->AddSkillXP(RawXP, MonsterLevel);
}

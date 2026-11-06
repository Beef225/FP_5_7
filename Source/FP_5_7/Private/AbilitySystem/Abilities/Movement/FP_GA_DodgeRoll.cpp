// Copyright JG

#include "AbilitySystem/Abilities/Movement/FP_GA_DodgeRoll.h"
#include "FP_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Engine/World.h"

UFP_GA_DodgeRoll::UFP_GA_DodgeRoll()
{
	StartupInputTag = FFP_GameplayTags::Get().InputTag_Space;
	FGameplayTagContainer NewTags;
	NewTags.AddTag(FFP_GameplayTags::Get().Skills_Movement_DodgeRoll);
	SetAssetTags(NewTags);

	InstancingPolicy  = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

EFP_RollDirection UFP_GA_DodgeRoll::GetRollDirection() const
{
	const ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char) return EFP_RollDirection::Forward;

	// Prefer velocity; fall back to input vector when standing still.
	FVector MoveDir = Char->GetVelocity();
	if (MoveDir.IsNearlyZero())
		MoveDir = Char->GetCharacterMovement()->GetLastInputVector();
	if (MoveDir.IsNearlyZero())
		return EFP_RollDirection::Forward;

	MoveDir.Normalize();

	const float F = FVector::DotProduct(MoveDir, Char->GetActorForwardVector());
	const float R = FVector::DotProduct(MoveDir, Char->GetActorRightVector());

	if (FMath::Abs(F) >= FMath::Abs(R))
		return F >= 0.f ? EFP_RollDirection::Forward : EFP_RollDirection::Backward;
	else
		return R >= 0.f ? EFP_RollDirection::Right : EFP_RollDirection::Left;
}

void UFP_GA_DodgeRoll::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// Clear anything a previous roll leaked before this one starts.
	RemoveAllMoveSpeedEffects();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// The BP graph may have ended the ability synchronously (e.g. commit failed).
	if (!IsActive()) return;

	if (const UWorld* World = GetWorld())
	{
		// Montages play at 1x (the BP montage task uses Rate 1); padding covers blend-out.
		const float Timeout = GetLongestMontageLength() + SafetyTimeoutPadding;
		World->GetTimerManager().SetTimer(SafetyTimerHandle,
			FTimerDelegate::CreateUObject(this, &UFP_GA_DodgeRoll::OnSafetyTimeout), Timeout, false);
	}
}

void UFP_GA_DodgeRoll::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (const UWorld* World = GetWorld())
		World->GetTimerManager().ClearTimer(SafetyTimerHandle);

	// Always drop the speed GE, however the ability ends (completed, interrupted, cancelled).
	RemoveAllMoveSpeedEffects();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFP_GA_DodgeRoll::OnSafetyTimeout()
{
	if (!IsActive()) return;

	UE_LOG(LogTemp, Warning,
		TEXT("GA_DodgeRoll: still active %.2fs after the longest roll montage — force-ending. "
		     "Check that the montage task's OnInterrupted / OnCancelled pins call End Ability."),
		SafetyTimeoutPadding);
	K2_CancelAbility();
}

float UFP_GA_DodgeRoll::GetLongestMontageLength() const
{
	float Longest = 0.f;
	for (const UAnimMontage* M : { Montage_Forward.Get(), Montage_Backward.Get(), Montage_Left.Get(), Montage_Right.Get() })
	{
		if (M) Longest = FMath::Max(Longest, M->GetPlayLength());
	}
	return Longest > 0.f ? Longest : 1.f;
}

void UFP_GA_DodgeRoll::RemoveAllMoveSpeedEffects()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && MoveSpeedEffectClass)
	{
		FGameplayEffectQuery Query;
		Query.EffectDefinition = MoveSpeedEffectClass;
		ASC->RemoveActiveEffects(Query);
	}
	else if (ASC && MoveSpeedHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(MoveSpeedHandle);
	}
	MoveSpeedHandle.Invalidate();
}

void UFP_GA_DodgeRoll::ApplyMoveSpeedEffect(float InitialMagnitude)
{
	if (!MoveSpeedEffectClass) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	// Never stack: a second apply in one activation would orphan the first handle.
	if (MoveSpeedHandle.IsValid())
		RemoveMoveSpeedEffect();

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(MoveSpeedEffectClass, GetAbilityLevel());
	SpecHandle.Data->SetSetByCallerMagnitude(FFP_GameplayTags::Get().SetByCaller_DodgeRoll_MoveSpeed, InitialMagnitude);

	MoveSpeedHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

void UFP_GA_DodgeRoll::UpdateMoveSpeedMagnitude(float NewMagnitude)
{
	if (!MoveSpeedHandle.IsValid()) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	ASC->UpdateActiveGameplayEffectSetByCallerMagnitude(
		MoveSpeedHandle,
		FFP_GameplayTags::Get().SetByCaller_DodgeRoll_MoveSpeed,
		NewMagnitude);
}

void UFP_GA_DodgeRoll::RemoveMoveSpeedEffect()
{
	if (!MoveSpeedHandle.IsValid()) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC) ASC->RemoveActiveGameplayEffect(MoveSpeedHandle);

	MoveSpeedHandle.Invalidate();
}

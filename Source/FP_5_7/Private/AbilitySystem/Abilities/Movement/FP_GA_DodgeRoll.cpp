// Copyright JG

#include "AbilitySystem/Abilities/Movement/FP_GA_DodgeRoll.h"
#include "FP_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"

UFP_GA_DodgeRoll::UFP_GA_DodgeRoll()
{
	StartupInputTag = FFP_GameplayTags::Get().InputTag_Space;
	AbilityTags.AddTag(FFP_GameplayTags::Get().Skills_Movement_DodgeRoll);

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

void UFP_GA_DodgeRoll::ApplyMoveSpeedEffect(float InitialMagnitude)
{
	if (!MoveSpeedEffectClass) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

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

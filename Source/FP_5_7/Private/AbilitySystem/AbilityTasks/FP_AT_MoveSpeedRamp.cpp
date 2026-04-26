// Copyright JG

#include "AbilitySystem/AbilityTasks/FP_AT_MoveSpeedRamp.h"
#include "AbilitySystem/Abilities/Movement/FP_GA_DodgeRoll.h"
#include "Animation/AnimMontage.h"

UFP_AT_MoveSpeedRamp* UFP_AT_MoveSpeedRamp::MoveSpeedRamp(
	UGameplayAbility* OwningAbility,
	UAnimMontage* Montage,
	float StartMagnitude,
	float EndMagnitude)
{
	UFP_AT_MoveSpeedRamp* Task = NewAbilityTask<UFP_AT_MoveSpeedRamp>(OwningAbility);
	Task->TargetMontage  = Montage;
	Task->StartMag       = StartMagnitude;
	Task->EndMag         = EndMagnitude;
	return Task;
}

void UFP_AT_MoveSpeedRamp::Activate()
{
	if (!TargetMontage)
	{
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	Duration = FMath::Max(TargetMontage->GetPlayLength(), 0.01f);
	bTickingTask = true;
}

void UFP_AT_MoveSpeedRamp::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	Elapsed += DeltaTime;
	const float Alpha = FMath::Clamp(Elapsed / Duration, 0.f, 1.f);
	const float CurrentMag = FMath::Lerp(StartMag, EndMag, Alpha);

	if (UFP_GA_DodgeRoll* DodgeRoll = Cast<UFP_GA_DodgeRoll>(Ability))
	{
		DodgeRoll->UpdateMoveSpeedMagnitude(CurrentMag);
	}

	if (Elapsed >= Duration)
	{
		OnFinished.Broadcast();
		EndTask();
	}
}

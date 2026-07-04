// Copyright JG

#include "AbilitySystem/AbilityTasks/FP_AT_LeapToLocation.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UFP_AT_LeapToLocation::UFP_AT_LeapToLocation()
{
	bTickingTask      = true;
	bSimulatedTask    = false;
	ElapsedTime       = 0.f;
	FallWaitElapsed   = 0.f;
	Duration          = 0.6f;
	ArcHeight         = 300.f;
	LandingClearance  = 40.f;
	MaxFallWaitTime   = 1.f;
	bWaitingForLanded = false;
	bMovementRestored = false;
}

UFP_AT_LeapToLocation* UFP_AT_LeapToLocation::LeapToLocation(
	UGameplayAbility* OwningAbility,
	FVector           InTargetLocation,
	float             InDuration,
	float             InArcHeight,
	float             InLandingClearance,
	float             InMaxFallWaitTime)
{
	UFP_AT_LeapToLocation* Task = NewAbilityTask<UFP_AT_LeapToLocation>(OwningAbility);
	Task->TargetLocation    = InTargetLocation;
	Task->Duration          = FMath::Max(InDuration, 0.05f);
	Task->ArcHeight         = InArcHeight;
	Task->LandingClearance  = FMath::Max(InLandingClearance, 0.f);
	Task->MaxFallWaitTime   = FMath::Max(InMaxFallWaitTime, 0.1f);
	return Task;
}

void UFP_AT_LeapToLocation::Activate()
{
	ACharacter* Character = GetCharacter();
	if (!Character)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
			OnCancelled.Broadcast();
		EndTask();
		return;
	}

	StartLocation     = Character->GetActorLocation();
	ArcTargetLocation = TargetLocation + FVector(0.f, 0.f, LandingClearance);

	UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
	CMC->SetMovementMode(MOVE_Flying);
	CMC->Velocity = FVector::ZeroVector;
}

void UFP_AT_LeapToLocation::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	ACharacter* Character = GetCharacter();
	if (!Character)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
			OnCancelled.Broadcast();
		EndTask();
		return;
	}

	if (bWaitingForLanded)
	{
		// Safety net in case Landed never fires (e.g. leapt somewhere ungrounded) —
		// force-finish rather than leaving the ability stuck.
		FallWaitElapsed += DeltaTime;
		if (FallWaitElapsed >= MaxFallWaitTime)
		{
			FinishLanding();
		}
		return;
	}

	ElapsedTime += DeltaTime;
	const float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.f, 1.f);

	// Lerp XYZ toward the clearance point (above the true landing spot) then add a
	// sine arc on top of the interpolated Z.
	const FVector Base      = FMath::Lerp(StartLocation, ArcTargetLocation, Alpha);
	const float   ArcOffset = ArcHeight * FMath::Sin(Alpha * UE_PI);
	const FVector NewPos(Base.X, Base.Y, Base.Z + ArcOffset);

	// Zero velocity each tick so CMC doesn't fight our position
	Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	Character->SetActorLocation(NewPos, false, nullptr, ETeleportType::TeleportPhysics);

	if (Alpha >= 1.f)
	{
		// Hand off to real gravity for the last LandingClearance cm so
		// CharacterMovementComponent's own landing detection picks the touchdown
		// frame, instead of hard-teleporting straight onto (or through) the floor.
		Character->SetActorLocation(ArcTargetLocation, false, nullptr, ETeleportType::TeleportPhysics);

		// Carry the arc's actual exit velocity into the fall instead of zeroing it —
		// the sine arc is moving fastest (both horizontally and vertically) right at
		// touchdown, so a hard zero here reads as the character slamming to a stop
		// mid-air, and the resulting near-zero-velocity fall crawls the last few cm
		// before CMC's floor sweep settles (visible as jitter).
		const FVector LinearVelocity = (ArcTargetLocation - StartLocation) / Duration;
		const float   ArcVelocityZ   = ArcHeight * (UE_PI / Duration) * FMath::Cos(Alpha * UE_PI);
		const FVector ExitVelocity   = LinearVelocity + FVector(0.f, 0.f, ArcVelocityZ);

		BeginFall(Character, ExitVelocity);
	}
}

void UFP_AT_LeapToLocation::BeginFall(ACharacter* Character, const FVector& ExitVelocity)
{
	UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
	CMC->Velocity = ExitVelocity;
	CMC->SetMovementMode(MOVE_Falling);

	Character->LandedDelegate.AddDynamic(this, &UFP_AT_LeapToLocation::OnCharacterLanded);
	bWaitingForLanded = true;
	FallWaitElapsed   = 0.f;
}

void UFP_AT_LeapToLocation::OnCharacterLanded(const FHitResult& Hit)
{
	FinishLanding();
}

void UFP_AT_LeapToLocation::FinishLanding()
{
	if (bMovementRestored) return;

	if (ACharacter* Character = GetCharacter())
		Character->LandedDelegate.RemoveDynamic(this, &UFP_AT_LeapToLocation::OnCharacterLanded);

	bWaitingForLanded = false;
	RestoreMovement();

	if (ShouldBroadcastAbilityTaskDelegates())
		OnCompleted.Broadcast();

	EndTask();
}

void UFP_AT_LeapToLocation::OnDestroy(bool bInOwnerFinished)
{
	if (bWaitingForLanded)
	{
		if (ACharacter* Character = GetCharacter())
			Character->LandedDelegate.RemoveDynamic(this, &UFP_AT_LeapToLocation::OnCharacterLanded);
		bWaitingForLanded = false;
	}
	RestoreMovement();
	Super::OnDestroy(bInOwnerFinished);
}

void UFP_AT_LeapToLocation::RestoreMovement()
{
	if (bMovementRestored) return;
	bMovementRestored = true;

	if (ACharacter* Character = GetCharacter())
	{
		UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
		if (CMC->MovementMode == MOVE_Flying || CMC->MovementMode == MOVE_Falling)
			CMC->SetMovementMode(MOVE_Walking);
	}
}

ACharacter* UFP_AT_LeapToLocation::GetCharacter() const
{
	return Cast<ACharacter>(GetAvatarActor());
}

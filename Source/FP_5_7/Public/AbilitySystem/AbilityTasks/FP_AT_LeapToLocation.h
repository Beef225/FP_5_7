// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FP_AT_LeapToLocation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFP_LeapToLocationDelegate);

/**
 * Moves the owning character to TargetLocation over Duration seconds with an arc,
 * then finishes the last LandingClearance cm as a real gravity fall so
 * CharacterMovementComponent's own landing detection decides the exact touchdown
 * frame — avoids hard-teleporting straight onto the floor (capsule/ground pop) and
 * lets a landing montage/VFX started from OnCompleted line up with an actual footfall.
 * Bypasses all root motion — positions character directly each tick during the arc.
 *
 * BP usage:
 *   Leap To Location
 *     ├─ Target Location    = ClampLeapTarget(hit location)  — the true, standing-height landing spot
 *     ├─ Duration           = 0.6
 *     ├─ Arc Height         = 300
 *     └─ Landing Clearance  = 40   — arc ends this far above TargetLocation, then falls the rest
 *   OnCompleted → [explosion / VFX] → EndAbility   (fires once actually landed, not on arc completion)
 */
UCLASS()
class FP_5_7_API UFP_AT_LeapToLocation : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFP_AT_LeapToLocation();

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks",
		meta=(DisplayName="Leap To Location", HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UFP_AT_LeapToLocation* LeapToLocation(
		UGameplayAbility* OwningAbility,
		FVector           TargetLocation,
		float             Duration         = 0.6f,
		float             ArcHeight        = 300.f,
		float             LandingClearance = 40.f,
		float             MaxFallWaitTime  = 1.f);

	/** Fires once the character has actually touched down (real landing, not just arc completion). */
	UPROPERTY(BlueprintAssignable)
	FFP_LeapToLocationDelegate OnCompleted;

	/** Fires if the task is cancelled before completion. */
	UPROPERTY(BlueprintAssignable)
	FFP_LeapToLocationDelegate OnCancelled;

	virtual void Activate()                     override;
	virtual void TickTask(float DeltaTime)      override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	FVector StartLocation;
	FVector TargetLocation;    // true landing spot (standing height)
	FVector ArcTargetLocation; // TargetLocation + LandingClearance in Z — where the scripted arc hands off to gravity
	float   Duration;
	float   ArcHeight;
	float   LandingClearance;
	float   MaxFallWaitTime;
	float   ElapsedTime;
	float   FallWaitElapsed;
	bool    bWaitingForLanded;
	bool    bMovementRestored;

	ACharacter* GetCharacter() const;
	void        BeginFall(ACharacter* Character, const FVector& ExitVelocity);
	void        FinishLanding();
	void        RestoreMovement();

	UFUNCTION()
	void OnCharacterLanded(const FHitResult& Hit);
};

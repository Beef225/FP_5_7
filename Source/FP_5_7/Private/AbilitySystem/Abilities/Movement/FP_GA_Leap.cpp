// Copyright JG

#include "AbilitySystem/Abilities/Movement/FP_GA_Leap.h"
#include "FP_GameplayTags.h"
#include "Interaction/FP_EnemyInterface.h"
#include "Characters/FP_PlayerCharacter.h"
#include "Player/FP_PlayerState.h"
#include "Engine/OverlapResult.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"

UFP_GA_Leap::UFP_GA_Leap()
{
	FGameplayTagContainer NewTags;
	NewTags.AddTag(FFP_GameplayTags::Get().Skills_Movement_Leap);
	SetAssetTags(NewTags);

	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UFP_GA_Leap::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// Freeze camera elevation for the whole windup → leap → landing sequence so the
	// crouch anims don't dip the camera. Released in EndAbility with a smooth blend.
	if (AFP_PlayerCharacter* PlayerChar = Cast<AFP_PlayerCharacter>(GetAvatarActorFromActorInfo()))
		PlayerChar->LockCameraElevation();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UFP_GA_Leap::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AFP_PlayerCharacter* PlayerChar = Cast<AFP_PlayerCharacter>(GetAvatarActorFromActorInfo()))
		PlayerChar->UnlockCameraElevation(CameraReleaseInterpSpeed);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ---------------------------------------------------------------------------
// Public BP-callable helpers
// ---------------------------------------------------------------------------

FVector UFP_GA_Leap::GetLeapTarget() const
{
	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return FVector::ZeroVector;

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!PC) return Character->GetActorLocation();

	FHitResult HitResult;
	if (!PC->GetHitResultUnderCursorByChannel(
			UEngineTypes::ConvertToTraceType(ECC_WorldStatic), true, HitResult))
		return Character->GetActorLocation();

	return ClampLeapTarget(HitResult.Location);
}

FVector UFP_GA_Leap::ClampLeapTarget(FVector RawHitLocation) const
{
	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return RawHitLocation;

	// Trace hits return the floor mesh's surface — but SetActorLocation places the
	// capsule's origin, not its feet. Without this, landing would sink the capsule
	// into the floor by half its height until CMC's own correction popped it back out.
	const float HalfHeight = Character->GetCapsuleComponent()
		? Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 0.f;

	const FVector CharLoc = Character->GetActorLocation();
	const float   Range   = GetEffectiveRange();

	const FVector2D ToTarget2D(RawHitLocation.X - CharLoc.X,
	                            RawHitLocation.Y - CharLoc.Y);

	if (ToTarget2D.Size() <= Range)
		return FVector(RawHitLocation.X, RawHitLocation.Y, RawHitLocation.Z + HalfHeight);

	// Beyond range — clamp XY, snap Z to ground
	const FVector2D Dir2D      = ToTarget2D.GetSafeNormal();
	const FVector   ClampedBase = CharLoc + FVector(Dir2D.X, Dir2D.Y, 0.f) * Range;

	FHitResult GroundHit;
	const FVector TraceStart(ClampedBase.X, ClampedBase.Y, CharLoc.Z + 500.f);
	const FVector TraceEnd  (ClampedBase.X, ClampedBase.Y, CharLoc.Z - 500.f);
	if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldStatic))
		return FVector(GroundHit.Location.X, GroundHit.Location.Y, GroundHit.Location.Z + HalfHeight);

	return FVector(ClampedBase.X, ClampedBase.Y, CharLoc.Z);
}

float UFP_GA_Leap::GetEffectiveRange() const
{
	float Extra = 0.f;
	if (const AFP_PlayerState* PS = GetLocalPS())
		Extra = PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Leap_ExtraRange);
	return BaseLeapRange + Extra;
}

bool UFP_GA_Leap::ShouldSkipWindup() const
{
	const AFP_PlayerState* PS = GetLocalPS();
	return PS && PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Leap_NoWindup) > 0.f;
}

bool UFP_GA_Leap::ShouldDoExplosiveLanding() const
{
	const AFP_PlayerState* PS = GetLocalPS();
	return PS && PS->GetSkillPassiveValue(FFP_GameplayTags::Get().SkillPassive_Leap_ExplosiveLanding) > 0.f;
}

void UFP_GA_Leap::ApplyLandingExplosion(const FVector& LandLocation)
{
	const float Radius = GetEffectiveSphereRadius(BaseExplosionRadius);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		LandLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(Radius));

	TArray<AActor*> HitActors;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Target = Overlap.GetActor();
		if (!Target || HitActors.Contains(Target)) continue;
		if (!Target->Implements<UFP_EnemyInterface>()) continue;

		HitActors.Add(Target);
		CauseDamage(Target);
	}
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

const AFP_PlayerState* UFP_GA_Leap::GetLocalPS() const
{
	if (const ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		if (const APlayerController* PC = Cast<APlayerController>(Char->GetController()))
			return PC->GetPlayerState<AFP_PlayerState>();
	return nullptr;
}

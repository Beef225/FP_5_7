// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_CharacterBase.h"
#include "Interaction/FP_PlayerInterface.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
class USoundBase;
class USkeletalMeshComponent;


UCLASS()
class FP_5_7_API AFP_PlayerCharacter : public AFP_CharacterBase, public IFP_PlayerInterface
{
	GENERATED_BODY()

public:
	AFP_PlayerCharacter();

	// Called by PlayerController when zoom input happens
	void AddCameraZoomInput(float ZoomDelta);

	/**
	 * Generic camera-follow override for movement skills (leap, dash, teleport, ...).
	 * Eases the camera boom toward Destination via VInterpTo instead of rigidly
	 * tracking the root, so anim-driven root motion (crouch dips, capsule resize)
	 * doesn't pop the camera. Call every tick for the duration of the effect — the
	 * destination is re-fed each time, so it can track a moving point (e.g. the
	 * character's live XY during a leap) or hold a fixed one (e.g. a teleport's
	 * end point). Raise InterpSpeed for skills whose travel speed scales with
	 * attributes so the camera doesn't lag behind. No-op on non-locally-controlled
	 * instances. Mirrored on UFP_AbilitySystemLibrary for use from any skill's BP.
	 */
	UFUNCTION(BlueprintCallable, Category="Camera")
	void InterpCameraToDestination(FVector Destination, float InterpSpeed, float DeltaTime);

	/**
	 * Releases a prior InterpCameraToDestination override. The camera eases back to
	 * its normal root-relative follow point at BlendOutSpeed — no hard snap, so a
	 * genuine elevation/position change picked up during the effect isn't lost.
	 */
	UFUNCTION(BlueprintCallable, Category="Camera")
	void ReleaseCameraInterp(float BlendOutSpeed = 10.f);

	/** Current world-space point the camera boom is easing toward (or sitting at, if settled). */
	UFUNCTION(BlueprintPure, Category="Camera")
	FVector GetCameraFollowLocation() const;

	/**
	 * Pins the boom's world Z at its value when called — exactly, with no lag,
	 * reacting to the root's TransformUpdated event rather than polling in Tick —
	 * while X/Y keep tracking the root normally (unaffected). Use for the common
	 * "don't let a crouch anim dip the camera" case. Call UnlockCameraElevation (or
	 * ReleaseCameraInterp) to release it with a smooth blend back to normal follow.
	 */
	UFUNCTION(BlueprintCallable, Category="Camera")
	void LockCameraElevation();

	UFUNCTION(BlueprintCallable, Category="Camera")
	void UnlockCameraElevation(float BlendOutSpeed = 10.f);

	/** Returns the skeletal mesh component for the given body part, or nullptr if unrecognised. */
	USkeletalMeshComponent* GetBodyPartMesh(EBodyPart BodyPart) const;

	/** Called by FFP_MeshFragment when a weapon with a left-hand IK socket is equipped. */
	void SetLeftHandIKTarget(USkeletalMeshComponent* WeaponMesh, FName SocketName, FVector JointTargetLocation);

	/** Called by FFP_MeshFragment on unequip. Clears the IK target. */
	void ClearLeftHandIKTarget();

	USkeletalMeshComponent* GetLeftHandIKWeaponMesh()    const { return LeftHandIKWeaponMesh.Get(); }
	FName                   GetLeftHandIKSocket()         const { return LeftHandIKSocket; }
	FVector                 GetLeftHandJointTargetLocation() const { return LeftHandJointTargetLocation; }
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	
	/** Combat Interface */
	virtual int32 GetPlayerLevel() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	/** end Combat Interface */

	/** Player Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InPoints) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual void LevelUp_Implementation() override;
	/** end Player Interface */

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	
	virtual void InitAbilityActorInfo() override;
	
	
	// -------------------------------------------------------------------------
	// Body Part Meshes
	// Each piece follows the main mesh via Set Leader Pose Component in Blueprint.
	// The main GetMesh() is set invisible at runtime; these pieces are visible.
	// -------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Body Parts", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh_Head;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Body Parts", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh_Torso;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Body Parts", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh_Arms;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Body Parts", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh_Hands;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Body Parts", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh_Legs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Body Parts", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh_Feet;

	// Components
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FX", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	UPROPERTY(EditDefaultsOnly, Category="FX", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundBase> LevelUpSound;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;

	// Zoom tuning
	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float MinArmLength = 350.f;

	// You requested max zoom out to be 800 and also the initial target length.
	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float MaxArmLength = 800.f;

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float ZoomStep = 120.f;

	// Base interp speed (we will scale it based on zoom level)
	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float ZoomInterpSpeed = 12.f;

	// Pitch tuning (optional, but included since you asked earlier)
	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float FarPitch = -60.f;   // when zoomed out

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float NearPitch = -35.f;  // when zoomed in

	UPROPERTY(EditDefaultsOnly, Category="Camera|Zoom")
	float PitchInterpSpeed = 10.f;

	// Runtime state
	float TargetArmLength = 800.f;

	// Camera follow-override state (see InterpCameraToDestination / ReleaseCameraInterp)
	bool    bCameraDestinationOverride = false;
	FVector CameraOverrideDestination  = FVector::ZeroVector;
	float   CameraOverrideInterpSpeed  = 10.f;
	float   CameraReleaseBlendSpeed    = 10.f;
	FVector BaseCameraBoomRelativeLocation = FVector::ZeroVector;

	// Elevation-lock state (see LockCameraElevation / UnlockCameraElevation)
	bool  bCameraElevationLocked = false;
	float LockedCameraWorldZ     = 0.f;

	// Mouse-facing
	UPROPERTY(EditDefaultsOnly, Category="Facing|Mouse")
	float FacingInterpSpeed = 0.f; // 0 = snap, >0 = smooth

	UPROPERTY(EditDefaultsOnly, Category="Facing|Mouse")
	float MinAimDistance = 3.f; // ignore tiny jitter when cursor is near feet

	bool GetMouseWorldPoint(FVector& OutWorldPoint) const;
	void FaceMouse(float DeltaTime);

	/**
	 * Bound to the root component's TransformUpdated delegate so the elevation lock
	 * corrects synchronously with whatever moved the root (CMC crouch, an AbilityTask
	 * on the ASC/PlayerState, physics, root motion — anything). Tick()-based
	 * correction isn't reliable here: the leap's AbilityTask ticks on a different
	 * actor (the ASC lives on PlayerState), so there's no guaranteed order against
	 * this actor's own Tick, which showed up as the camera trailing/rising through
	 * the leap's vertical arc. Reacting to the transform event itself sidesteps the
	 * ordering race entirely.
	 */
	void OnRootTransformUpdated(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);

	TWeakObjectPtr<USkeletalMeshComponent> LeftHandIKWeaponMesh;
	FName    LeftHandIKSocket            = NAME_None;
	FVector  LeftHandJointTargetLocation = FVector::ZeroVector;

	bool bIsFrozen = false;
	bool bRotationCancelled = false;

	FVector MovementLockDir = FVector::ZeroVector;

public:
	FVector GetMovementLockDir() const { return MovementLockDir; }

private:
	UFUNCTION()
	void OnFrozenTagChangedForFacing(FGameplayTag Tag, int32 NewCount);

	UFUNCTION()
	void OnRotationCancelledTagChanged(FGameplayTag Tag, int32 NewCount);

	UFUNCTION()
	void OnDodgeRollTagChanged(FGameplayTag Tag, int32 NewCount);
};

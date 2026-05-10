// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Libraries/FP_EnumDefs.h"
#include "SaveSystem/FP_ProfileSaveData.h"
#include "FP_CharacterPreviewActor.generated.h"

class USkeletalMeshComponent;
class UFP_UpperBodyStanceSet;
class FFP_PreviewInputProcessor;

/**
 * Lightweight character display actor for main menu / character select screens.
 * Mirrors the player character's body part mesh setup and can load equipment
 * visuals directly from an inventory save — no player state or ability system required.
 *
 * Grip stance is stored locally and fed to UFP_PreviewAnimInstance each frame.
 */
UCLASS(BlueprintType, Blueprintable)
class FP_5_7_API AFP_CharacterPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	AFP_CharacterPreviewActor();

	/**
	 * Loads inventory save for the given character record and applies equipment meshes
	 * and grip stance. No-op if no save exists for this character.
	 */
	UFUNCTION(BlueprintCallable, Category="Preview")
	void LoadFromCharacterRecord(const FFP_CharacterSaveRecord& CharacterRecord);

	/** Clear all equipped visuals and revert to default meshes / Unarmed stance. */
	UFUNCTION(BlueprintCallable, Category="Preview")
	void ResetToDefault();

	/** Directly override the displayed grip stance (e.g. for UI previewing). */
	UFUNCTION(BlueprintCallable, Category="Preview")
	void SetPreviewGripStance(EWeaponGripStyle InGripStance);

	EWeaponGripStyle  GetPreviewGripStance() const { return PreviewGripStance; }
	UFP_UpperBodyStanceSet* GetStanceSet()   const { return StanceSet; }

	USkeletalMeshComponent* GetLeftHandIKWeaponMesh()        const { return LeftHandIKWeaponMesh.Get(); }
	FName                   GetLeftHandIKSocket()            const { return LeftHandIKSocket; }
	FVector                 GetLeftHandJointTargetLocation() const { return LeftHandJointTargetLocation; }

	/** Drag sensitivity — degrees of yaw per pixel dragged. */
	UPROPERTY(EditDefaultsOnly, Category="Preview|Rotation")
	float RotationSpeed = 0.25f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	// ---- Animation config --------------------------------------------------

	/** Stance data asset — maps EWeaponGripStyle to upper-body AnimSequences. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Preview|Animation")
	TObjectPtr<UFP_UpperBodyStanceSet> StanceSet;

	// ---- Default meshes (shown when no save is loaded) ---------------------

	UPROPERTY(EditDefaultsOnly, Category="Preview|Defaults")
	TObjectPtr<USkeletalMesh> Default_Head;

	UPROPERTY(EditDefaultsOnly, Category="Preview|Defaults")
	TObjectPtr<USkeletalMesh> Default_Torso;

	UPROPERTY(EditDefaultsOnly, Category="Preview|Defaults")
	TObjectPtr<USkeletalMesh> Default_Arms;

	UPROPERTY(EditDefaultsOnly, Category="Preview|Defaults")
	TObjectPtr<USkeletalMesh> Default_Hands;

	UPROPERTY(EditDefaultsOnly, Category="Preview|Defaults")
	TObjectPtr<USkeletalMesh> Default_Legs;

	UPROPERTY(EditDefaultsOnly, Category="Preview|Defaults")
	TObjectPtr<USkeletalMesh> Default_Feet;

	// ---- Body part mesh components -----------------------------------------
	// Mesh_Body is the animation root; all others follow it via SetLeaderPoseComponent.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh_Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh_Head;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh_Torso;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh_Arms;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh_Hands;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh_Legs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Preview|Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh_Feet;

private:
	UPROPERTY()
	EWeaponGripStyle PreviewGripStance = EWeaponGripStyle::Unarmed;

	/** Socket-attached weapon/accessory components spawned from the save. Cleared on ResetToDefault. */
	UPROPERTY()
	TArray<TObjectPtr<USkeletalMeshComponent>> SpawnedAttachments;

	UPROPERTY()
	TWeakObjectPtr<USkeletalMeshComponent> LeftHandIKWeaponMesh;

	FName    LeftHandIKSocket            = NAME_None;
	FVector  LeftHandJointTargetLocation = FVector::ZeroVector;

	void ClearAttachments();
	USkeletalMeshComponent* GetBodyPartMesh(EBodyPart BodyPart) const;

	TSharedPtr<FFP_PreviewInputProcessor> InputProcessor;
	bool  bWasMouseDown = false;
	float LastMouseX    = 0.f;
};

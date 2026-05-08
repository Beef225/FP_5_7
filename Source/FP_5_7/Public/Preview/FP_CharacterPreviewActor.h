// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_CharacterPreviewActor.generated.h"

class USkeletalMeshComponent;
class UFP_UpperBodyStanceSet;
class UFP_InventorySaveData;

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

	/** Load equipment meshes and grip stance from an inventory save record. */
	UFUNCTION(BlueprintCallable, Category="Preview")
	void LoadFromInventorySave(UFP_InventorySaveData* SaveData);

	/** Clear all equipped visuals and revert to default meshes / Unarmed stance. */
	UFUNCTION(BlueprintCallable, Category="Preview")
	void ResetToDefault();

	/** Directly override the displayed grip stance (e.g. for UI previewing). */
	UFUNCTION(BlueprintCallable, Category="Preview")
	void SetPreviewGripStance(EWeaponGripStyle InGripStance);

	EWeaponGripStyle  GetPreviewGripStance() const { return PreviewGripStance; }
	UFP_UpperBodyStanceSet* GetStanceSet()   const { return StanceSet; }

protected:
	virtual void BeginPlay() override;

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

	void ClearAttachments();
	USkeletalMeshComponent* GetBodyPartMesh(EBodyPart BodyPart) const;
};

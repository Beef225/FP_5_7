// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FP_ItemActor.generated.h"

class UFP_ItemComponent;
class UFP_ItemFragmentTemplate;
class UStaticMeshComponent;
class UNiagaraSystem;
class UNiagaraComponent;
struct FFP_ItemManifest;

/**
 * World actor representing a dropped or placed inventory item.
 *
 * Components:
 *  - Static mesh for the item's visual representation
 *  - UFP_ItemComponent carrying the FFP_ItemManifest (item data)
 *  - Optional Niagara component for spawn / idle VFX
 *
 * Registers itself with UFP_ItemLabelManager on BeginPlay so a screen-space
 * pickup label is created and tracked automatically. The label calls
 * OnPickupRequested when the player clicks it.
 */
UCLASS()
class FP_5_7_API AFP_ItemActor : public AActor
{
	GENERATED_BODY()

public:

	AFP_ItemActor();

	/** Called by UFP_ItemPickupWidget when the player presses the pickup button. */
	void OnPickupRequested();

	/**
	 * Stamps fragments from FragmentTemplate into this item's manifest.
	 * Only adds fragments whose tag is not already present — existing data is never overwritten.
	 * Appears as a button in the Blueprint Class Defaults panel.
	 */
	UFUNCTION(CallInEditor, Category="Item|Setup")
	void ApplyFragmentTemplate();

	const FFP_ItemManifest& GetItemManifest() const;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	void RegisterWithLabelManager();
	void UnregisterFromLabelManager();

	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	void SnapToGround();

	UPROPERTY(EditAnywhere, Category = "Item")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Offset applied to the mesh relative to the actor root — use to reorient dropped items per Blueprint. */
	UPROPERTY(EditAnywhere, Category = "Item")
	FRotator MeshRotationOffset{ FRotator::ZeroRotator };

	UPROPERTY(EditAnywhere, Category = "Item")
	FVector MeshLocationOffset{ FVector::ZeroVector };

	/** Speed of the upward loot pop on spawn. */
	UPROPERTY(EditAnywhere, Category = "Item|Physics")
	float LootPopStrength{ 400.f };

	/** Max random horizontal spread on the loot pop. */
	UPROPERTY(EditAnywhere, Category = "Item|Physics")
	float LootPopSpread{ 150.f };

	bool bSettled{ false };

	FTimerHandle SettleTimeoutHandle;

	/** Carries the FFP_ItemManifest — set this up in the Blueprint Details panel. */
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<UFP_ItemComponent> ItemComponent;

	/** Template to stamp onto this item's manifest via the Apply Template button. */
	UPROPERTY(EditAnywhere, Category="Item|Setup")
	TObjectPtr<UFP_ItemFragmentTemplate> FragmentTemplate;

	/** Optional Niagara system played on spawn / as an idle loop. */
	UPROPERTY(EditAnywhere, Category = "Item|VFX")
	TObjectPtr<UNiagaraSystem> SpawnEffect;

	UPROPERTY(VisibleAnywhere, Category = "Item|VFX")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
};

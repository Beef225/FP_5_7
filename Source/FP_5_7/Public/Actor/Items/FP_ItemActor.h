// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FP_ItemActor.generated.h"

class UFP_ItemComponent;
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

	const FFP_ItemManifest& GetItemManifest() const;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	void RegisterWithLabelManager();
	void UnregisterFromLabelManager();

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Carries the FFP_ItemManifest — set this up in the Blueprint Details panel. */
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<UFP_ItemComponent> ItemComponent;

	/** Optional Niagara system played on spawn / as an idle loop. */
	UPROPERTY(EditAnywhere, Category = "Item|VFX")
	TObjectPtr<UNiagaraSystem> SpawnEffect;

	UPROPERTY(VisibleAnywhere, Category = "Item|VFX")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
};

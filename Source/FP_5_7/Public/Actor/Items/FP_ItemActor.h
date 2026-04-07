// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FP_ItemActor.generated.h"

class UFP_ItemComponent;
class UFP_ItemPickupWidget;
class UStaticMeshComponent;
class UWidgetComponent;
class UNiagaraSystem;
class UNiagaraComponent;

/**
 * World actor representing a dropped or placed inventory item.
 *
 * Components:
 *  - Static mesh for the item's visual representation
 *  - Widget component hosting UFP_ItemPickupWidget in world space
 *  - UFP_ItemComponent carrying the FFP_ItemManifest (item data)
 *  - Optional Niagara component for spawn / idle VFX
 *
 * When the player clicks the pickup widget button, OnPickupRequested is called,
 * which routes the item into the inventory via UFP_InventoryComponent::TryAddItem.
 */
UCLASS()
class FP_5_7_API AFP_ItemActor : public AActor
{
	GENERATED_BODY()

public:

	AFP_ItemActor();

	/** Called by UFP_ItemPickupWidget when the player presses the pickup button. */
	void OnPickupRequested();

protected:

	virtual void BeginPlay() override;

private:

	void InitialisePickupWidget();

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<UWidgetComponent> WidgetComponent;

	/** Carries the FFP_ItemManifest — set this up in the Blueprint Details panel. */
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<UFP_ItemComponent> ItemComponent;

	/** Optional Niagara system played on spawn / as an idle loop. */
	UPROPERTY(EditAnywhere, Category = "Item|VFX")
	TObjectPtr<UNiagaraSystem> SpawnEffect;

	UPROPERTY(VisibleAnywhere, Category = "Item|VFX")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Item")
	TSubclassOf<UFP_ItemPickupWidget> PickupWidgetClass;
};

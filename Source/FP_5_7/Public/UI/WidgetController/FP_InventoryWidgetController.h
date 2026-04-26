// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_WidgetController.h"
#include "FP_InventoryWidgetController.generated.h"

class UFP_InventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFP_OnNoRoomInInventory);

/**
 * Widget controller for the inventory UI.
 * Bridges UFP_InventoryComponent events to Blueprint-bindable delegates.
 */
UCLASS(BlueprintType, Blueprintable)
class FP_5_7_API UFP_InventoryWidgetController : public UFP_WidgetController
{
	GENERATED_BODY()

public:

	virtual void BindCallbacksToDependencies() override;

	/** Fired when an item pickup attempt fails due to a full inventory. Bind in BP to show a message. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FFP_OnNoRoomInInventory OnNoRoomInInventory;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ShowInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void HideInventory();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsInventoryVisible() const;

private:

	UFP_InventoryComponent* GetInventoryComponent() const;

	UFUNCTION()
	void OnNoRoom();
};

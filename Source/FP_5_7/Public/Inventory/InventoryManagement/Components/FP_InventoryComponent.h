// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Items/FP_ItemComponent.h"
#include "Inventory/InventoryManagement/FastArray/FP_FastArray.h"
#include "FP_InventoryComponent.generated.h"

class UFP_InventoryBase;
class UFP_InventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFP_InventoryItemChange, UFP_InventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFP_NoRoomInInventory);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class FP_5_7_API UFP_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFP_InventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FFP_InventoryItemChange OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FFP_InventoryItemChange OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FFP_NoRoomInInventory NoRoomInInventory;

	/** Server-only. Attempts to add the item held by ItemComponent to this inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void TryAddItem(UFP_ItemComponent* ItemComponent);

	UFUNCTION(Server, Reliable)
	void Server_AddNewItem(UFP_ItemComponent* ItemComponent, int32 StackCount);

	UFUNCTION(Server, Reliable)
	void Server_AddStacksToItem(UFP_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddRepSubObj(UObject* SubObj);

	UPROPERTY(Replicated)
	FFP_InventoryFastArray InventoryList;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ShowInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void HideInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsInventoryVisible() const;

protected:

	virtual void BeginPlay() override;

private:

	TWeakObjectPtr<APlayerController> OwningController;

	void ConstructInventory();

	UPROPERTY()
	TObjectPtr<UFP_InventoryBase> InventoryMenu;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFP_InventoryBase> InventoryMenuClass;
};

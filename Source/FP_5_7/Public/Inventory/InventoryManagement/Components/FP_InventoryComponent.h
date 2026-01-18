// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FP_InventoryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class FP_5_7_API UFP_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UFP_InventoryComponent();

protected:
	
	virtual void BeginPlay() override;

private:
	
	TWeakObjectPtr<APlayerController> OwningController;
	
	void ConstructInventory();
	
	//UPROPERTY()
	//TObjectPtr<UFP_InventoryBase>InventoryMenu;
};

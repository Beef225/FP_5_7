// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "FP_ItemComponent.generated.h"

/**
 * Component placed on world pickup actors.
 * Carries the FFP_ItemManifest that describes what inventory item this pickup creates.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FP_5_7_API UFP_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FFP_ItemManifest GetItemManifest() const { return ItemManifest; }

private:

	UPROPERTY(Replicated, EditAnywhere, Category = "Inventory")
	FFP_ItemManifest ItemManifest;
};

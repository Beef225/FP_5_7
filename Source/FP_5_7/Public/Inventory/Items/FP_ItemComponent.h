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

	UFP_ItemComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const FFP_ItemManifest& GetItemManifest() const { return ItemManifest; }
	FFP_ItemManifest& GetItemManifestMutable() { return ItemManifest; }

	/** Called server-side when the item is fully picked up. Fires OnPickedUp then destroys the owning actor. */
	void PickedUp();

	/** Initialises the manifest from a copy — used when spawning a dropped item actor. */
	void InitItemManifest(FFP_ItemManifest CopyOfManifest);

protected:

	/** Override in Blueprint to play a pickup effect or sound before the actor is destroyed. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnPickedUp();

private:

	UPROPERTY(Replicated, EditAnywhere, Category = "Inventory")
	FFP_ItemManifest ItemManifest;
};

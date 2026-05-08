// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_GripStanceFragment.generated.h"

/**
 * Applies a weapon grip stance to the player when the item is equipped.
 * Reverts to Unarmed on unequip.
 * Only valid on weapon items.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_GripStanceFragment : public FFP_ItemFragment
{
	GENERATED_BODY()

	void OnEquip(APlayerController* PC);
	void OnUnequip(APlayerController* PC);

	EWeaponGripStyle GetGripStance() const { return GripStance; }

private:

	UPROPERTY(EditAnywhere, Category = "Weapon")
	EWeaponGripStyle GripStance{ EWeaponGripStyle::Unarmed };
};

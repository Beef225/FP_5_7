// Copyright JG

#include "Inventory/Items/Fragments/FP_GripStanceFragment.h"
#include "Player/FP_PlayerState.h"

void FFP_GripStanceFragment::OnEquip(APlayerController* PC)
{
	if (!PC) return;
	if (AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>())
		PS->SetGripStance(GripStance);
}

void FFP_GripStanceFragment::OnUnequip(APlayerController* PC)
{
	if (!PC) return;
	if (AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>())
		PS->SetGripStance(EWeaponGripStyle::Unarmed);
}

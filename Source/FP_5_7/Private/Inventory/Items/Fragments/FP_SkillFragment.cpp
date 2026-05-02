// Copyright JG

#include "Inventory/Items/Fragments/FP_SkillFragment.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Player/FP_PlayerState.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf_Skills.h"

void FFP_SkillFragment::OnSpawned(FFP_ItemManifest& Manifest)
{
	if (bRolled || SkillPool.IsEmpty()) return;
	bRolled = true;

	// Fisher-Yates shuffle on indices, then take first GrantCount.
	TArray<int32> Indices;
	Indices.Reserve(SkillPool.Num());
	for (int32 i = 0; i < SkillPool.Num(); ++i) Indices.Add(i);

	for (int32 i = Indices.Num() - 1; i > 0; --i)
	{
		const int32 j = FMath::RandRange(0, i);
		Indices.Swap(i, j);
	}

	const int32 Count = FMath::Min(GrantCount, SkillPool.Num());
	for (int32 i = 0; i < Count; ++i)
	{
		const FFP_SkillPoolEntry& Entry = SkillPool[Indices[i]];
		if (Entry.SkillTag.IsValid())
		{
			GrantedSkillTags.Add(Entry.SkillTag);
			GrantedSkillLevels.Add(Entry.SkillLevel);
		}
	}
}

void FFP_SkillFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	if (GrantedSkillTags.IsEmpty()) return;

	FFP_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UFP_Leaf_Skills* Leaf = Cast<UFP_Leaf_Skills>(Composite);
	if (!IsValid(Leaf)) return;

	const APlayerController* PC = Leaf->GetOwningPlayer();
	const AFP_PlayerState* PS = PC ? PC->GetPlayerState<AFP_PlayerState>() : nullptr;
	Leaf->SetSkills(GrantedSkillTags, PS, SkillLibrary);
}

void FFP_SkillFragment::OnEquip(APlayerController* PC)
{
	if (GrantedSkillTags.IsEmpty() || !PC) return;

	AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>();
	if (!PS) return;

	const UFP_SkillLibrary* Library = PS->GetSkillLibrary();
	if (!Library) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn());
	if (!ASI) return;

	UFP_AbilitySystemComponent* ASC = Cast<UFP_AbilitySystemComponent>(ASI->GetAbilitySystemComponent());
	if (!ASC) return;

	for (int32 i = 0; i < GrantedSkillTags.Num(); ++i)
	{
		const FFP_AbilityEntry Entry = Library->FindAbilityEntryForTag(GrantedSkillTags[i], true);
		if (!Entry.SkillAbility)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SKILL EQUIP] Tag '%s' has no SkillAbility set in SkillLibrary"), *GrantedSkillTags[i].ToString());
			continue;
		}

		const int32 Level = PS->GetSkillLevel(GrantedSkillTags[i]);
		const FGameplayAbilitySpecHandle Handle = ASC->GrantItemSkill(Entry.SkillAbility, Level);
		if (Handle.IsValid())
		{
			ActiveHandles.Add(Handle);
			// Restore any saved hotbar slot assignment for this newly-granted skill.
			PS->ApplyInputTagForSkill(ASC, GrantedSkillTags[i]);
		}

		PS->AddGrantedSkill(GrantedSkillTags[i]);
	}
}

void FFP_SkillFragment::OnUnequip(APlayerController* PC)
{
	if (!PC) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn());
	if (!ASI) return;

	UFP_AbilitySystemComponent* ASC = Cast<UFP_AbilitySystemComponent>(ASI->GetAbilitySystemComponent());
	if (!ASC) return;

	for (FGameplayAbilitySpecHandle& Handle : ActiveHandles)
		ASC->RevokeItemSkill(Handle);

	ActiveHandles.Reset();

	if (AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>())
	{
		for (const FGameplayTag& Tag : GrantedSkillTags)
			PS->RemoveGrantedSkill(Tag);
	}
}

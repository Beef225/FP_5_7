// Copyright JG

#include "Inventory/Items/Fragments/FP_ItemNameFragment.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "Inventory/Items/Fragments/FP_AffixFragment.h"
#include "Inventory/Items/Affixes/FP_AffixTypes.h"
#include "FP_GameplayTags.h"
#include "UI/Widget/Inventory/Composite/FP_CompositeBase.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf_Text.h"

void FFP_ItemNameFragment::OnSpawned(FFP_ItemManifest& Manifest)
{
	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();

	const FFP_RarityFragment* RarityFrag = Manifest.GetFragmentOfType<FFP_RarityFragment>();
	CachedRarity = RarityFrag ? RarityFrag->GetRarity() : EFP_ItemRarity::Common;

	FString TypeName;
	if (const FFP_TextFragment* TypeFrag = Manifest.GetFragmentOfTypeWithTag<FFP_TextFragment>(Tags.Fragment_ItemType))
	{
		TypeName = TypeFrag->GetText().ToString();
	}

	if (CachedRarity == EFP_ItemRarity::Common)
	{
		GeneratedName = FText::FromString(TypeName);
		return;
	}

	if (CachedRarity == EFP_ItemRarity::Uncommon)
	{
		FString PrefixName;
		FString SuffixName;

		if (const FFP_AffixFragment* AffixFrag = Manifest.GetFragmentOfType<FFP_AffixFragment>())
		{
			for (const FFP_AffixInstance& Affix : AffixFrag->GetRolledAffixes())
			{
				if (PrefixName.IsEmpty() && Affix.AffixType == EFP_AffixType::Prefix)
					PrefixName = Affix.TierName.ToString();
				if (SuffixName.IsEmpty() && Affix.AffixType == EFP_AffixType::Suffix)
					SuffixName = Affix.TierName.ToString();
				if (!PrefixName.IsEmpty() && !SuffixName.IsEmpty())
					break;
			}
		}

		FString Name = TypeName;
		if (!PrefixName.IsEmpty())
			Name = PrefixName + TEXT(" ") + Name;
		if (!SuffixName.IsEmpty())
			Name = Name + TEXT(" ") + SuffixName;

		GeneratedName = FText::FromString(Name);
		return;
	}

	// Rare: pick two random words from the name table for this item type
	if (CachedRarity == EFP_ItemRarity::Rare && IsValid(RareNameTable))
	{
		const FName Prefix = RareNameTable->PickPrefix();
		const FName Suffix = RareNameTable->PickSuffix();

		FString Name;
		if (Prefix != NAME_None && Suffix != NAME_None)
			Name = Prefix.ToString() + TEXT(" ") + Suffix.ToString();
		else if (Prefix != NAME_None)
			Name = Prefix.ToString();
		else
			Name = TypeName;

		GeneratedName = FText::FromString(Name);
		return;
	}

	// Legendary / Mythic — placeholder until dedicated naming systems are built
	if (CachedRarity == EFP_ItemRarity::Legendary)
	{
		GeneratedName = FText::FromString(TEXT("LEGENDARY NAME TBD"));
		return;
	}

	GeneratedName = FText::FromString(TypeName);
}

void FFP_ItemNameFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();
	const FGameplayTag WidgetTag = Composite->GetFragmentTag();

	if (WidgetTag.MatchesTagExact(Tags.Fragment_ItemType))
	{
		if (CachedRarity == EFP_ItemRarity::Common || CachedRarity == EFP_ItemRarity::Uncommon)
			Composite->Collapse();
		else
			Composite->Expand();
		return;
	}

	// Fragment.ItemName — expand and populate
	FFP_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	if (UFP_Leaf_Text* LeafText = Cast<UFP_Leaf_Text>(Composite))
	{
		LeafText->SetText(GeneratedName);
	}
}

// Copyright JG

#include "Inventory/Items/Fragments/FP_AffixFragment.h"
#include "Inventory/Items/Affixes/FP_AffixDefinition.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf_Affixes.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "GameFramework/PlayerController.h"

void FFP_AffixFragment::OnSpawned(FFP_ItemManifest& Manifest)
{
	if (bRolled || !AffixPool || !Registry) return;
	bRolled = true;

	if (const FFP_ItemLevelFragment* LevelFrag = Manifest.GetFragmentOfType<FFP_ItemLevelFragment>())
	{
		ItemLevel = LevelFrag->GetItemLevel();
	}

	if (const FFP_RarityFragment* RarityFrag = Manifest.GetFragmentOfType<FFP_RarityFragment>())
	{
		MaxPrefixes = RarityFrag->GetRolledPrefixCount();
		MaxSuffixes = RarityFrag->GetRolledSuffixCount();
	}

	RollAffixes();
}

void FFP_AffixFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	FFP_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UFP_Leaf_Affixes* Leaf = Cast<UFP_Leaf_Affixes>(Composite);
	if (!IsValid(Leaf)) return;

	// Sort: prefixes first, then by pool position so the same affix types always
	// appear in the same slot on any item using this pool.
	TArray<FFP_AffixInstance> Sorted = RolledAffixes;

	TMap<FName, int32> PoolOrder;
	if (IsValid(AffixPool))
	{
		for (int32 i = 0; i < AffixPool->Entries.Num(); ++i)
			PoolOrder.Add(AffixPool->Entries[i].AffixID, i);
	}

	Sorted.Sort([&](const FFP_AffixInstance& A, const FFP_AffixInstance& B)
	{
		if (A.AffixType != B.AffixType)
			return A.AffixType == EFP_AffixType::Prefix;
		const int32* IdxA = PoolOrder.Find(A.AffixID);
		const int32* IdxB = PoolOrder.Find(B.AffixID);
		return (IdxA ? *IdxA : INT32_MAX) < (IdxB ? *IdxB : INT32_MAX);
	});

	Leaf->SetAffixes(Sorted);
}

void FFP_AffixFragment::OnEquip(APlayerController* PC)
{
	if (RolledAffixes.IsEmpty() || !PC) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn());
	if (!ASI) return;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;

	const UFP_AttributeSet* AS = Cast<UFP_AttributeSet>(ASC->GetAttributeSet(UFP_AttributeSet::StaticClass()));
	if (!AS) return;

	UGameplayEffect* DynamicGE = NewObject<UGameplayEffect>(GetTransientPackage(), NAME_None);
	DynamicGE->DurationPolicy = EGameplayEffectDurationType::Infinite;

	auto AddMod = [&](FGameplayTag StatTag, float Value)
	{
		if (!StatTag.IsValid()) return;
		const TStaticFuncPtr<FGameplayAttribute()>* AttrFunc = AS->TagsToAttributes.Find(StatTag);
		if (!AttrFunc) return;

		FGameplayModifierInfo Mod;
		Mod.Attribute   = (*AttrFunc)();
		Mod.ModifierOp  = EGameplayModOp::Additive;
		FScalableFloat SF;
		SF.Value        = Value;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SF);
		DynamicGE->Modifiers.Add(Mod);
	};

	for (const FFP_AffixInstance& Affix : RolledAffixes)
	{
		AddMod(Affix.Stat1_Attr, Affix.Stat1_Value);
		if (Affix.HasStat2())
			AddMod(Affix.Stat2_Attr, Affix.Stat2_Value);
	}

	if (DynamicGE->Modifiers.IsEmpty()) return;

	ActiveEffectHandle = ASC->ApplyGameplayEffectToSelf(DynamicGE, 1.f, ASC->MakeEffectContext());
}

void FFP_AffixFragment::OnUnequip(APlayerController* PC)
{
	if (!ActiveEffectHandle.IsValid() || !PC) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn());
	if (!ASI) return;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;

	ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
	ActiveEffectHandle.Invalidate();
}

void FFP_AffixFragment::RollAffixes()
{
	TSet<FName> UsedGroups;
	FName LockedAffinity = NAME_None;
	int32 PrefixCount = 0;
	int32 SuffixCount = 0;

	auto RollStatValue = [](float Min, float Max, float Step) -> float
	{
		if (Step <= 0.f || Max <= Min) return Min;
		const int32 Steps = FMath::RoundToInt32((Max - Min) / Step);
		return Min + FMath::RandRange(0, Steps) * Step;
	};

	while (PrefixCount < MaxPrefixes || SuffixCount < MaxSuffixes)
	{
		// Build eligible pool for this pick
		TArray<const FFP_AffixPoolEntry*> Eligible;
		int32 TotalWeight = 0;

		for (const FFP_AffixPoolEntry& Entry : AffixPool->Entries)
		{
			if (UsedGroups.Contains(Entry.GroupTag)) continue;

			if (!Entry.AffinityTag.IsNone() && !LockedAffinity.IsNone()
				&& Entry.AffinityTag != LockedAffinity) continue;

			const UFP_AffixDefinition* Def = Registry->Find(Entry.AffixID);
			if (!Def) continue;

			if (Def->Type == EFP_AffixType::Prefix && PrefixCount >= MaxPrefixes) continue;
			if (Def->Type == EFP_AffixType::Suffix && SuffixCount >= MaxSuffixes) continue;

			if (!Def->PickTier(ItemLevel)) continue;

			Eligible.Add(&Entry);
			TotalWeight += Entry.PoolWeight;
		}

		if (Eligible.IsEmpty() || TotalWeight <= 0) break;

		// Weighted pick from eligible pool
		int32 Roll = FMath::RandRange(0, TotalWeight - 1);
		int32 Cumulative = 0;
		const FFP_AffixPoolEntry* Picked = nullptr;
		for (const FFP_AffixPoolEntry* Entry : Eligible)
		{
			Cumulative += Entry->PoolWeight;
			if (Roll < Cumulative) { Picked = Entry; break; }
		}
		if (!Picked) continue;

		const UFP_AffixDefinition* Def = Registry->Find(Picked->AffixID);
		if (!Def) continue;

		const FFP_AffixTierData* TierData = Def->PickTier(ItemLevel);
		if (!TierData) continue;

		FFP_AffixInstance Instance;
		Instance.AffixID       = Picked->AffixID;
		Instance.AffixType     = Def->Type;
		Instance.GroupTag      = Picked->GroupTag;
		Instance.Tier          = TierData->Tier;
		Instance.TierName      = TierData->ItemName;
		Instance.DisplayFormat = Def->DisplayFormat;
		Instance.Stat1_Attr         = Def->Stat1_Attr;
		Instance.Stat1_Value        = RollStatValue(TierData->Stat1_Min, TierData->Stat1_Max, TierData->Stat1_Step);
		Instance.Stat1_DecimalCount = TierData->Stat1_DecimalCount;
		Instance.Stat1_DisplayAs    = TierData->Stat1_DisplayAs;

		if (TierData->HasStat2())
		{
			Instance.Stat2_Attr         = Def->Stat2_Attr;
			Instance.Stat2_Value        = RollStatValue(TierData->Stat2_Min, TierData->Stat2_Max, TierData->Stat2_Step);
			Instance.Stat2_DecimalCount = TierData->Stat2_DecimalCount;
			Instance.Stat2_DisplayAs    = TierData->Stat2_DisplayAs;
		}

		RolledAffixes.Add(Instance);
		UsedGroups.Add(Picked->GroupTag);

		if (!Picked->AffinityTag.IsNone() && LockedAffinity.IsNone())
		{
			LockedAffinity = Picked->AffinityTag;
		}

		if (Def->Type == EFP_AffixType::Prefix) PrefixCount++;
		else SuffixCount++;
	}
}

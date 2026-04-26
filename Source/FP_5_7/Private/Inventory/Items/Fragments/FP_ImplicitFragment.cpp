// Copyright JG

#include "Inventory/Items/Fragments/FP_ImplicitFragment.h"
#include "Inventory/Items/Affixes/FP_AffixDefinition.h"
#include "Inventory/Items/Fragments/FP_AffixFragment.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf_Implicits.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "GameFramework/PlayerController.h"

void FFP_ImplicitFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	if (RolledImplicits.IsEmpty()) return;

	FFP_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UFP_Leaf_Implicits* Leaf = Cast<UFP_Leaf_Implicits>(Composite);
	if (!IsValid(Leaf)) return;

	Leaf->SetImplicits(RolledImplicits);
}

struct FFP_ImplicitStatSnapshot
{
	FGameplayAttribute Attr;
	FString            TagName;
	float              Before{ 0.f };
};

static TArray<FFP_ImplicitStatSnapshot> SnapshotImplicitStats(
	const TArray<FFP_AffixInstance>& Implicits,
	const UFP_AttributeSet* AS,
	const UAbilitySystemComponent* ASC)
{
	TArray<FFP_ImplicitStatSnapshot> Snaps;
	for (const FFP_AffixInstance& Implicit : Implicits)
	{
		auto TrySnap = [&](FGameplayTag Tag)
		{
			if (!Tag.IsValid()) return;
			const TStaticFuncPtr<FGameplayAttribute()>* Fn = AS->TagsToAttributes.Find(Tag);
			if (!Fn)
			{
				UE_LOG(LogTemp, Warning, TEXT("[IMPLICIT] Tag '%s' has no entry in TagsToAttributes — will not be logged"), *Tag.ToString());
				return;
			}
			FFP_ImplicitStatSnapshot S;
			S.Attr    = (*Fn)();
			S.TagName = Tag.ToString();
			S.Before  = ASC->GetNumericAttribute(S.Attr);
			Snaps.Add(S);
		};
		TrySnap(Implicit.Stat1_Attr);
		if (Implicit.HasStat2()) TrySnap(Implicit.Stat2_Attr);
	}
	return Snaps;
}

static void LogImplicitStatChanges(
	const TCHAR* Phase,
	const TArray<FFP_ImplicitStatSnapshot>& Snaps,
	const UAbilitySystemComponent* ASC)
{
	UE_LOG(LogTemp, Log, TEXT("[IMPLICIT %s] Attribute changes:"), Phase);
	for (const FFP_ImplicitStatSnapshot& S : Snaps)
	{
		const float After = ASC->GetNumericAttribute(S.Attr);
		UE_LOG(LogTemp, Log, TEXT("  %-55s  %.4f  ->  %.4f  (delta %+.4f)"),
			*S.TagName, S.Before, After, After - S.Before);
	}
}

void FFP_ImplicitFragment::OnEquip(APlayerController* PC)
{
	if (RolledImplicits.IsEmpty() || !PC) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn());
	if (!ASI) return;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;

	const UFP_AttributeSet* AS = Cast<UFP_AttributeSet>(ASC->GetAttributeSet(UFP_AttributeSet::StaticClass()));
	if (!AS) return;

	TArray<FFP_ImplicitStatSnapshot> Snaps = SnapshotImplicitStats(RolledImplicits, AS, ASC);

	UGameplayEffect* DynamicGE = NewObject<UGameplayEffect>(GetTransientPackage(), NAME_None);
	DynamicGE->DurationPolicy = EGameplayEffectDurationType::Infinite;

	auto AddMod = [&](FGameplayTag StatTag, float Value)
	{
		if (!StatTag.IsValid()) return;
		const TStaticFuncPtr<FGameplayAttribute()>* AttrFunc = AS->TagsToAttributes.Find(StatTag);
		if (!AttrFunc)
		{
			UE_LOG(LogTemp, Warning, TEXT("[IMPLICIT EQUIP] Tag '%s' has no entry in TagsToAttributes — stat will not be applied"), *StatTag.ToString());
			return;
		}

		FGameplayModifierInfo Mod;
		Mod.Attribute         = (*AttrFunc)();
		Mod.ModifierOp        = EGameplayModOp::Additive;
		FScalableFloat SF;
		SF.Value              = Value;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SF);
		DynamicGE->Modifiers.Add(Mod);
	};

	for (const FFP_AffixInstance& Implicit : RolledImplicits)
	{
		AddMod(Implicit.Stat1_Attr, Implicit.Stat1_Value);
		if (Implicit.HasStat2())
			AddMod(Implicit.Stat2_Attr, Implicit.Stat2_Value);
	}

	if (DynamicGE->Modifiers.IsEmpty()) return;

	ActiveEffectHandle = ASC->ApplyGameplayEffectToSelf(DynamicGE, 1.f, ASC->MakeEffectContext());
	if (FFP_AffixFragment::bLogStatChanges) LogImplicitStatChanges(TEXT("EQUIP"), Snaps, ASC);
}

void FFP_ImplicitFragment::OnUnequip(APlayerController* PC)
{
	if (!ActiveEffectHandle.IsValid() || !PC) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn());
	if (!ASI) return;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;

	const UFP_AttributeSet* AS = Cast<UFP_AttributeSet>(ASC->GetAttributeSet(UFP_AttributeSet::StaticClass()));
	if (!AS) return;

	TArray<FFP_ImplicitStatSnapshot> Snaps = SnapshotImplicitStats(RolledImplicits, AS, ASC);

	ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
	ActiveEffectHandle.Invalidate();

	if (FFP_AffixFragment::bLogStatChanges) LogImplicitStatChanges(TEXT("UNEQUIP"), Snaps, ASC);
}

void FFP_ImplicitFragment::OnSpawned(FFP_ItemManifest& Manifest)
{
	if (bRolled) return;
	bRolled = true;

	auto RollStatValue = [](float Min, float Max, float Step) -> float
	{
		if (Step <= 0.f || Max <= Min) return Min;
		const int32 Steps = FMath::RoundToInt32((Max - Min) / Step);
		return Min + FMath::RandRange(0, Steps) * Step;
	};

	for (const FFP_ImplicitEntry& Entry : Entries)
	{
		if (!Entry.AffixDef) continue;

		const FFP_AffixTierData* TierData = nullptr;
		for (const FFP_AffixTierData& T : Entry.AffixDef->Tiers)
		{
			if (T.Tier == Entry.Tier)
			{
				TierData = &T;
				break;
			}
		}
		if (!TierData) continue;

		FFP_AffixInstance Instance;
		Instance.AffixID            = Entry.AffixDef->AffixID;
		Instance.AffixType          = Entry.AffixDef->Type;
		Instance.GroupTag           = NAME_None;
		Instance.Tier               = TierData->Tier;
		Instance.TierName           = TierData->ItemName;
		Instance.DisplayFormat      = Entry.AffixDef->DisplayFormat;
		Instance.Stat1_Attr         = Entry.AffixDef->Stat1_Attr;
		Instance.Stat1_Value        = RollStatValue(TierData->Stat1_Min, TierData->Stat1_Max, TierData->Stat1_Step);
		Instance.Stat1_DecimalCount = TierData->Stat1_DecimalCount;
		Instance.Stat1_DisplayAs    = TierData->Stat1_DisplayAs;

		if (TierData->HasStat2())
		{
			Instance.Stat2_Attr         = Entry.AffixDef->Stat2_Attr;
			Instance.Stat2_Value        = RollStatValue(TierData->Stat2_Min, TierData->Stat2_Max, TierData->Stat2_Step);
			Instance.Stat2_DecimalCount = TierData->Stat2_DecimalCount;
			Instance.Stat2_DisplayAs    = TierData->Stat2_DisplayAs;
		}

		RolledImplicits.Add(Instance);
	}
}

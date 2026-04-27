// Copyright JG

#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Characters/FP_PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Inventory/Loot/Data/FP_RarityTable.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "Player/FP_PlayerState.h"
#include "UI/Widget/Inventory/Composite/FP_CompositeBase.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf_AttributeRequirements.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf_Image.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf_LabeledValue.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf_Text.h"

void FFP_InventoryItemFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}

bool FFP_InventoryItemFragment::MatchesWidgetTag(const UFP_CompositeBase* Composite) const
{
	return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());
}

void FFP_TextFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	FFP_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UFP_Leaf_Text* LeafText = Cast<UFP_Leaf_Text>(Composite);
	if (!IsValid(LeafText)) return;

	LeafText->SetText(FragmentText);
}

void FFP_LabeledNumberFragment::OnSpawned(FFP_ItemManifest& Manifest)
{
	if (bRandomizeOnManifest)
	{
		Value = FMath::FRandRange(Min, Max);
		bRandomizeOnManifest = false;
	}
}

void FFP_LabeledNumberFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	FFP_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UFP_Leaf_LabeledValue* LabeledValue = Cast<UFP_Leaf_LabeledValue>(Composite);
	if (!IsValid(LabeledValue)) return;

	LabeledValue->SetText_Label(Text_Label, bCollapseLabel);

	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = MinFractionalDigits;
	Options.MaximumFractionalDigits = MaxFractionalDigits;
	LabeledValue->SetText_Value(FText::AsNumber(Value, &Options), bCollapseValue);
}

void FFP_ConsumableFragment::OnConsume(APlayerController* PC)
{
	for (TInstancedStruct<FFP_ConsumeModifier>& Modifier : Modifiers)
	{
		if (FFP_ConsumeModifier* Ptr = Modifier.GetMutablePtr<FFP_ConsumeModifier>())
		{
			Ptr->OnConsume(PC);
		}
	}
}

void FFP_ConsumableFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	FFP_InventoryItemFragment::Assimilate(Composite);
	for (const TInstancedStruct<FFP_ConsumeModifier>& Modifier : Modifiers)
	{
		if (const FFP_ConsumeModifier* Ptr = Modifier.GetPtr<FFP_ConsumeModifier>())
		{
			Composite->ApplyFunction([Ptr](UFP_CompositeBase* Widget)
			{
				Ptr->Assimilate(Widget);
			});
		}
	}
}

void FFP_ConsumableFragment::Manifest()
{
	for (TInstancedStruct<FFP_ConsumeModifier>& Modifier : Modifiers)
	{
		if (FFP_ConsumeModifier* Ptr = Modifier.GetMutablePtr<FFP_ConsumeModifier>())
		{
			Ptr->Manifest();
		}
	}
}

void FFP_ConsumableFragment::OnSpawned(FFP_ItemManifest& Manifest)
{
	for (TInstancedStruct<FFP_ConsumeModifier>& Modifier : Modifiers)
	{
		if (FFP_ConsumeModifier* Ptr = Modifier.GetMutablePtr<FFP_ConsumeModifier>())
		{
			Ptr->OnSpawned(Manifest);
		}
	}
}

void FFP_MeshFragment::OnEquip(APlayerController* PC)
{
	if (bEquipped) return;

	AFP_PlayerCharacter* Character = PC ? Cast<AFP_PlayerCharacter>(PC->GetPawn()) : nullptr;
	if (!Character) return;

	for (const FFP_MeshEntry& Entry : MeshEntries)
	{
		if (Entry.bReplaceMesh)
		{
			// Body-part replacement / hide
			if (Entry.BodyPart == EBodyPart::None) continue;

			USkeletalMeshComponent* BodyMesh = Character->GetBodyPartMesh(Entry.BodyPart);
			if (!BodyMesh) continue;

			// Cache the original only once per body part (guard against duplicate entries)
			if (!CachedOriginalMeshes.Contains(Entry.BodyPart))
			{
				CachedOriginalMeshes.Add(Entry.BodyPart, BodyMesh->GetSkeletalMeshAsset());
			}

			// Null mesh = hide; non-null = replace
			BodyMesh->SetSkeletalMeshAsset(Entry.Mesh);
		}
		else
		{
			// Socket attachment — Mesh_Torso is the source of truth for all sockets.
			if (!IsValid(Entry.Mesh) || Entry.Socket == NAME_None) continue;

			USkeletalMeshComponent* AttachParent = Character->GetBodyPartMesh(EBodyPart::Torso);
			if (!AttachParent) continue;

			USkeletalMeshComponent* SocketMesh = NewObject<USkeletalMeshComponent>(Character);
			SocketMesh->SetSkeletalMeshAsset(Entry.Mesh);
			SocketMesh->RegisterComponent();
			SocketMesh->AttachToComponent(
				AttachParent,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				Entry.Socket);

			SpawnedSocketComponents.Add(SocketMesh);
		}
	}

	bEquipped = true;
}

void FFP_MeshFragment::OnUnequip(APlayerController* PC)
{
	if (!bEquipped) return;

	AFP_PlayerCharacter* Character = PC ? Cast<AFP_PlayerCharacter>(PC->GetPawn()) : nullptr;
	if (!Character) return;

	// Restore all replaced body part meshes to their originals
	for (const TTuple<EBodyPart, TObjectPtr<USkeletalMesh>>& Cached : CachedOriginalMeshes)
	{
		USkeletalMeshComponent* BodyMesh = Character->GetBodyPartMesh(Cached.Key);
		if (BodyMesh)
		{
			BodyMesh->SetSkeletalMeshAsset(Cached.Value);
		}
	}
	CachedOriginalMeshes.Empty();

	// Destroy all socket-attached add-on meshes
	for (TWeakObjectPtr<USkeletalMeshComponent>& WeakComp : SpawnedSocketComponents)
	{
		if (WeakComp.IsValid())
		{
			WeakComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			WeakComp->DestroyComponent();
		}
	}
	SpawnedSocketComponents.Empty();

	bEquipped = false;
}

void FFP_StrengthModifier::OnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Green,
		FString::Printf(TEXT("Strength increased by: %f"), GetValue()));
}

void FFP_StrengthModifier::OnUnequip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Green,
		FString::Printf(TEXT("Item unequipped. Strength decreased by: %f"), GetValue()));
}

void FFP_EquipmentFragment::OnEquip(APlayerController* PC)
{
	if (bEquipped) return;
	bEquipped = true;
	for (TInstancedStruct<FFP_EquipModifier>& Modifier : EquipModifiers)
	{
		if (FFP_EquipModifier* Ptr = Modifier.GetMutablePtr<FFP_EquipModifier>())
		{
			Ptr->OnEquip(PC);
		}
	}
}

void FFP_EquipmentFragment::OnUnequip(APlayerController* PC)
{
	if (!bEquipped) return;
	bEquipped = false;
	for (TInstancedStruct<FFP_EquipModifier>& Modifier : EquipModifiers)
	{
		if (FFP_EquipModifier* Ptr = Modifier.GetMutablePtr<FFP_EquipModifier>())
		{
			Ptr->OnUnequip(PC);
		}
	}
}

void FFP_EquipmentFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	FFP_InventoryItemFragment::Assimilate(Composite);
	for (const TInstancedStruct<FFP_EquipModifier>& Modifier : EquipModifiers)
	{
		if (const FFP_EquipModifier* Ptr = Modifier.GetPtr<FFP_EquipModifier>())
		{
			Ptr->Assimilate(Composite);
		}
	}
}

void FFP_ImageFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	FFP_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UFP_Leaf_Image* Image = Cast<UFP_Leaf_Image>(Composite);
	if (!IsValid(Image)) return;

	Image->SetImage(Icon);
	Image->SetBoxSize(IconDimensions);
	Image->SetImageSize(IconDimensions);
}

void FFP_RarityFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	FFP_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UFP_Leaf_Text* LeafText = Cast<UFP_Leaf_Text>(Composite);
	if (!IsValid(LeafText)) return;

	const FString RarityString = StaticEnum<EFP_ItemRarity>()
		->GetDisplayNameTextByValue(static_cast<int64>(Rarity)).ToString().ToUpper();
	LeafText->SetText(FText::FromString(RarityString));
}

void FFP_RarityFragment::OnSpawned(FFP_ItemManifest& Manifest)
{
	if (bRolled || !IsValid(RarityTable)) return;
	bRolled = true;

	float TotalIIR = 0.f;

	if (const APlayerController* PC = GWorld ? GWorld->GetFirstPlayerController() : nullptr)
	{
		if (const AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>())
		{
			if (const UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
			{
				TotalIIR += ASC->GetNumericAttribute(UFP_AttributeSet::GetItemRarityAttribute());
			}
		}
	}

	// TODO: add monster difficulty IIR modifier
	// TODO: add area level IIR modifier

	Rarity = RarityTable->Roll(TotalIIR);

	if (const FFP_RarityWeight* Entry = RarityTable->GetEntry(Rarity))
	{
		RolledPrefixCount = FMath::RandRange(Entry->MinPrefixes, Entry->MaxPrefixes);
		RolledSuffixCount = FMath::RandRange(Entry->MinSuffixes, Entry->MaxSuffixes);
	}

	if (Rarity == EFP_ItemRarity::Legendary)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange,
				TEXT("CONGRATULATIONS! YOU DROPPED A LEGENDARY!"));
		}
	}
}

// -------------------------------------------------------------------------
// Attribute Requirements
// -------------------------------------------------------------------------

bool FFP_AttributeRequirementFragment::MeetsRequirements(APlayerController* PC) const
{
	if (!IsValid(PC)) return false;

	const AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>();
	if (!PS) return false;

	if (EffectiveRequiredLevel > 0 && PS->GetPlayerLevel() < EffectiveRequiredLevel) return false;

	const UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return false;

	auto Check = [&](int32 Required, FGameplayAttribute Attr) -> bool
	{
		return Required == 0 || ASC->GetNumericAttribute(Attr) >= static_cast<float>(Required);
	};

	return Check(RequiredMight,     UFP_AttributeSet::GetMightAttribute())
		&& Check(RequiredResonance, UFP_AttributeSet::GetResonanceAttribute())
		&& Check(RequiredAgility,   UFP_AttributeSet::GetAgilityAttribute())
		&& Check(RequiredFortitude, UFP_AttributeSet::GetFortitudeAttribute());
}

void FFP_AttributeRequirementFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;

	if (EffectiveRequiredLevel == 0 && RequiredMight == 0 && RequiredResonance == 0 && RequiredAgility == 0 && RequiredFortitude == 0)
		return;

	UFP_Leaf_AttributeRequirements* Leaf = Cast<UFP_Leaf_AttributeRequirements>(Composite);
	if (!IsValid(Leaf)) return;

	Composite->Expand();

	int32 CurrentLevel = 0, CurrentMight = 0, CurrentResonance = 0, CurrentAgility = 0, CurrentFortitude = 0;

	if (const APlayerController* PC = GWorld ? GWorld->GetFirstPlayerController() : nullptr)
	{
		if (const AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>())
		{
			CurrentLevel = PS->GetPlayerLevel();
			if (const UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
			{
				CurrentMight     = static_cast<int32>(ASC->GetNumericAttribute(UFP_AttributeSet::GetMightAttribute()));
				CurrentResonance = static_cast<int32>(ASC->GetNumericAttribute(UFP_AttributeSet::GetResonanceAttribute()));
				CurrentAgility   = static_cast<int32>(ASC->GetNumericAttribute(UFP_AttributeSet::GetAgilityAttribute()));
				CurrentFortitude = static_cast<int32>(ASC->GetNumericAttribute(UFP_AttributeSet::GetFortitudeAttribute()));
			}
		}
	}

	Leaf->SetRequirements(
		{ EffectiveRequiredLevel, CurrentLevel     },
		{ RequiredMight,          CurrentMight     },
		{ RequiredResonance,      CurrentResonance },
		{ RequiredAgility,        CurrentAgility   },
		{ RequiredFortitude,      CurrentFortitude }
	);
}

// -------------------------------------------------------------------------
// Item Level
// -------------------------------------------------------------------------

void FFP_ItemLevelFragment::Assimilate(UFP_CompositeBase* Composite) const
{
	FFP_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UFP_Leaf_LabeledValue* LabeledValue = Cast<UFP_Leaf_LabeledValue>(Composite);
	if (!IsValid(LabeledValue)) return;

	LabeledValue->SetText_Label(NSLOCTEXT("FP_ItemLevel", "Label", "Item Level"), false);

	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = 0;
	Options.MaximumFractionalDigits = 0;
	LabeledValue->SetText_Value(FText::AsNumber(ItemLevel, &Options), false);
}

// Copyright JG

#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "UI/Widget/Inventory/Composite/FP_CompositeBase.h"
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

void FFP_LabeledNumberFragment::OnSpawned()
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

void FFP_ConsumableFragment::OnSpawned()
{
	for (TInstancedStruct<FFP_ConsumeModifier>& Modifier : Modifiers)
	{
		if (FFP_ConsumeModifier* Ptr = Modifier.GetMutablePtr<FFP_ConsumeModifier>())
		{
			Ptr->OnSpawned();
		}
	}
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

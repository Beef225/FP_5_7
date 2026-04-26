// Copyright JG

#include "UI/Widget/Items/FP_ItemPickupWidget.h"
#include "Actor/Items/FP_ItemActor.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "Inventory/Items/Fragments/FP_ItemNameFragment.h"
#include "Libraries/FP_EnumDefs.h"
#include "Styling/SlateBrush.h"

void UFP_ItemPickupWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_Pickup)
	{
		Button_Pickup->OnClicked.AddDynamic(this, &ThisClass::OnPickupButtonClicked);
	}
}

void UFP_ItemPickupWidget::SetItemActor(AFP_ItemActor* InItemActor)
{
	ItemActor = InItemActor;

	if (!Text_ItemName) return;

	FText DisplayName = FText::FromString(TEXT("NAME NOT FOUND"));
	EFP_ItemRarity Rarity = EFP_ItemRarity::Common;

	if (IsValid(InItemActor))
	{
		const FFP_ItemManifest& Manifest = InItemActor->GetItemManifest();

		if (const FFP_ItemNameFragment* NameFrag = Manifest.GetFragmentOfType<FFP_ItemNameFragment>())
		{
			const FText Generated = NameFrag->GetGeneratedName();
			if (!Generated.IsEmpty())
				DisplayName = Generated;
			Rarity = NameFrag->GetCachedRarity();
		}
	}

	FLinearColor NameColor;
	switch (Rarity)
	{
	case EFP_ItemRarity::Uncommon:   NameColor = FLinearColor(0.15f, 0.75f, 0.15f, 1.f); break;
	case EFP_ItemRarity::Rare:       NameColor = FLinearColor(0.95f, 0.80f, 0.10f, 1.f); break;
	case EFP_ItemRarity::Legendary:  NameColor = FLinearColor(0.90f, 0.25f, 0.25f, 1.f); break;
	default:                         NameColor = FLinearColor::White;                      break;
	}

	Text_ItemName->SetText(DisplayName);
	Text_ItemName->SetFont(ItemNameFont);
	Text_ItemName->SetColorAndOpacity(FSlateColor(NameColor));
}

void UFP_ItemPickupWidget::SetBackgroundTint(const FLinearColor& Tint)
{
	if (!Image_Background) return;

	FSlateBrush Brush = Image_Background->GetBrush();
	Brush.TintColor = FSlateColor(Tint);
	Image_Background->SetBrush(Brush);
}

void UFP_ItemPickupWidget::OnPickupButtonClicked()
{
	if (ItemActor.IsValid())
	{
		ItemActor->OnPickupRequested();
	}
}

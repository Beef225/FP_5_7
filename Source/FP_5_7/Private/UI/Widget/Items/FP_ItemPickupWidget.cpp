// Copyright JG

#include "UI/Widget/Items/FP_ItemPickupWidget.h"
#include "Actor/Items/FP_ItemActor.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "FP_GameplayTags.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
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

	if (IsValid(InItemActor))
	{
		const FFP_ItemManifest& Manifest = InItemActor->GetItemManifest();
		if (const FFP_TextFragment* NameFragment = Manifest.GetFragmentOfTypeWithTag<FFP_TextFragment>(FFP_GameplayTags::Get().Fragment_ItemName))
		{
			const FText FragmentText = NameFragment->GetText();
			if (!FragmentText.IsEmpty())
			{
				DisplayName = FragmentText;
			}
		}
	}

	Text_ItemName->SetText(DisplayName);
	Text_ItemName->SetFont(ItemNameFont);
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

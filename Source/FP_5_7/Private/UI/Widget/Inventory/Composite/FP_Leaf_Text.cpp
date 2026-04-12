// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_Leaf_Text.h"
#include "Components/TextBlock.h"

void UFP_Leaf_Text::SetText(const FText& Text) const
{
	Text_LeafText->SetText(Text);
}

void UFP_Leaf_Text::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSlateFontInfo FontInfo = Text_LeafText->GetFont();
	FontInfo.Size = FontSize;
	Text_LeafText->SetFont(FontInfo);
}

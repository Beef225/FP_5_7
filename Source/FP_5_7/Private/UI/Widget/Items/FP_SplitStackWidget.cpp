// Copyright JG

#include "UI/Widget/Items/FP_SplitStackWidget.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UFP_SplitStackWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Slider_Split->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderValueChanged);
	Button_Confirm->OnClicked.AddDynamic(this, &ThisClass::OnConfirmClicked);
	Button_Cancel->OnClicked.AddDynamic(this, &ThisClass::OnCancelClicked);
}

void UFP_SplitStackWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	RemoveFromParent();
}

void UFP_SplitStackWidget::SetupForItem(UFP_InventoryItem* Item, int32 StackCount, int32 InGridIndex)
{
	InventoryItem = Item;
	MaxStackCount = StackCount;
	GridIndex = InGridIndex;

	Slider_Split->SetMinValue(1.f);
	Slider_Split->SetMaxValue(static_cast<float>(StackCount));
	Slider_Split->SetValue(1.f);

	Text_SplitAmount->SetText(FText::AsNumber(1));
}

void UFP_SplitStackWidget::OnSliderValueChanged(float Value)
{
	const int32 IntValue = FMath::RoundToInt(Value);
	Text_SplitAmount->SetText(FText::AsNumber(IntValue));
}

void UFP_SplitStackWidget::OnConfirmClicked()
{
	const int32 SplitAmount = FMath::RoundToInt(Slider_Split->GetValue());
	OnSplitConfirmed.Broadcast(SplitAmount, GridIndex);
	RemoveFromParent();
}

void UFP_SplitStackWidget::OnCancelClicked()
{
	OnSplitCancelled.Broadcast();
	RemoveFromParent();
}

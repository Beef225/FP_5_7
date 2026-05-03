// Copyright JG

#include "UI/Widget/HUD/FP_SkillPickerPopup.h"
#include "UI/Widget/HUD/FP_SkillFrame.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"

void UFP_SkillPickerPopup::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Backdrop)
		Button_Backdrop->OnClicked.AddDynamic(this, &UFP_SkillPickerPopup::OnBackdropClicked);
}

void UFP_SkillPickerPopup::Init(UFP_SkillFrame* InOwningFrame)
{
	OwningFrame = InOwningFrame;
}

void UFP_SkillPickerPopup::PositionContent(FVector2D AbsoluteAnchorPos)
{
	if (!Border_Content) return;
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Border_Content->Slot);
	if (!CanvasSlot) return;

	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(this);
	const FVector2D ViewportPos = (DPIScale > 0.f) ? AbsoluteAnchorPos / DPIScale : AbsoluteAnchorPos;

	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetPosition(ViewportPos);
	CanvasSlot->SetAlignment(FVector2D(0.5f, 1.f)); // bottom-centre of Border_Content → anchor point
}

void UFP_SkillPickerPopup::OnBackdropClicked()
{
	if (UFP_SkillFrame* Frame = OwningFrame.Get())
		Frame->ClosePickerPopup(); // handles RemoveFromParent + frame state cleanup
	else
		RemoveFromParent();
}

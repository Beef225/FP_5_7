// Copyright JG

#include "UI/Widget/HUD/FP_SkillFrameActions.h"
#include "UI/Widget/HUD/FP_SkillFrame.h"
#include "Components/Button.h"

void UFP_SkillFrameActions::Init(UFP_SkillFrame* InOwningFrame)
{
	OwningFrame = InOwningFrame;

	if (Button_ClearSlot)
		Button_ClearSlot->OnClicked.AddDynamic(this, &UFP_SkillFrameActions::OnClearSlotClicked);

	if (Button_MoveSlot)
		Button_MoveSlot->OnClicked.AddDynamic(this, &UFP_SkillFrameActions::OnMoveSlotClicked);
}

void UFP_SkillFrameActions::OnClearSlotClicked()
{
	if (UFP_SkillFrame* Frame = OwningFrame.Get())
		Frame->ClearSlot();
}

void UFP_SkillFrameActions::OnMoveSlotClicked()
{
	if (UFP_SkillFrame* Frame = OwningFrame.Get())
		Frame->BeginMoveSlot();
}

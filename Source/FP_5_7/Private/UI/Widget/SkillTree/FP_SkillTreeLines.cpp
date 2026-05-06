// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeLines.h"
#include "Rendering/DrawElements.h"

// ---------------------------------------------------------------------------
// SSkillTreeLines
// ---------------------------------------------------------------------------

void SSkillTreeLines::Construct(const FArguments& InArgs)
{
	Thickness       = InArgs._LineThickness;
	Color_Active    = InArgs._LineColor_Active;
	Color_Partial   = InArgs._LineColor_Partial;
	Color_Inactive  = InArgs._LineColor_Inactive;

	SetCanTick(false);
	SetVisibility(EVisibility::HitTestInvisible);
}

void SSkillTreeLines::SetConnectionData(
	const TArray<TTuple<FGameplayTag, FGameplayTag>>& InPairs,
	const TMap<FGameplayTag, FVector2D>& InPosMap)
{
	Pairs  = InPairs;
	PosMap = InPosMap;
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SSkillTreeLines::SetState(
	const FGameplayTagContainer& InAllocated,
	const TArray<FGameplayTag>&  InPending)
{
	AllocatedTags = InAllocated;
	PendingTags   = InPending;
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SSkillTreeLines::SetStyle(float InThickness, FLinearColor InActive, FLinearColor InPartial, FLinearColor InInactive)
{
	Thickness      = InThickness;
	Color_Active   = InActive;
	Color_Partial  = InPartial;
	Color_Inactive = InInactive;
	Invalidate(EInvalidateWidgetReason::Paint);
}

int32 SSkillTreeLines::OnPaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : Pairs)
	{
		const FVector2D* StartPtr = PosMap.Find(Pair.Get<0>());
		const FVector2D* EndPtr   = PosMap.Find(Pair.Get<1>());
		if (!StartPtr || !EndPtr) continue;

		const bool bAActive = AllocatedTags.HasTag(Pair.Get<0>()) || PendingTags.Contains(Pair.Get<0>());
		const bool bBActive = AllocatedTags.HasTag(Pair.Get<1>()) || PendingTags.Contains(Pair.Get<1>());

		FLinearColor Color;
		if      (bAActive && bBActive) Color = Color_Active;
		else if (bAActive || bBActive) Color = Color_Partial;
		else                           Color = Color_Inactive;

		const TArray<FVector2D> Points = { *StartPtr, *EndPtr };
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(),
			Points,
			ESlateDrawEffect::None,
			Color,
			true,
			Thickness);
	}

	return LayerId;
}


// ---------------------------------------------------------------------------
// UFP_SkillTreeLines
// ---------------------------------------------------------------------------

UFP_SkillTreeLines::UFP_SkillTreeLines()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UFP_SkillTreeLines::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	LinesWidget.Reset();
}

TSharedRef<SWidget> UFP_SkillTreeLines::RebuildWidget()
{
	LinesWidget = SNew(SSkillTreeLines)
		.LineThickness(LineThickness)
		.LineColor_Active(LineColor_Active)
		.LineColor_Partial(LineColor_Partial)
		.LineColor_Inactive(LineColor_Inactive);

	// Restore cached data if RebuildWidget is called after data was already set
	if (!CachedPairs.IsEmpty())
		LinesWidget->SetConnectionData(CachedPairs, CachedPosMap);
	if (CachedAllocated.Num() > 0 || !CachedPending.IsEmpty())
		LinesWidget->SetState(CachedAllocated, CachedPending);

	return LinesWidget.ToSharedRef();
}

void UFP_SkillTreeLines::SetConnectionData(
	const TArray<TTuple<FGameplayTag, FGameplayTag>>& InPairs,
	const TMap<FGameplayTag, FVector2D>& InPosMap)
{
	CachedPairs  = InPairs;
	CachedPosMap = InPosMap;
	if (LinesWidget.IsValid())
		LinesWidget->SetConnectionData(InPairs, InPosMap);
}

void UFP_SkillTreeLines::SetState(
	const FGameplayTagContainer& InAllocated,
	const TArray<FGameplayTag>&  InPending)
{
	CachedAllocated = InAllocated;
	CachedPending   = InPending;
	if (LinesWidget.IsValid())
		LinesWidget->SetState(InAllocated, InPending);
}

void UFP_SkillTreeLines::SetStyle(float InThickness, FLinearColor InActive, FLinearColor InPartial, FLinearColor InInactive)
{
	LineThickness     = InThickness;
	LineColor_Active  = InActive;
	LineColor_Partial = InPartial;
	LineColor_Inactive = InInactive;
	if (LinesWidget.IsValid())
		LinesWidget->SetStyle(InThickness, InActive, InPartial, InInactive);
}

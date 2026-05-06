// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "GameplayTagContainer.h"
#include "Widgets/SLeafWidget.h"
#include "FP_SkillTreeLines.generated.h"

// ---------------------------------------------------------------------------
// Slate leaf widget — does the actual drawing
// ---------------------------------------------------------------------------

class SSkillTreeLines : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillTreeLines)
		: _LineThickness(2.f)
		, _LineColor_Active(FLinearColor(1.f, 0.8f, 0.2f, 1.f))
		, _LineColor_Partial(FLinearColor(0.5f, 0.5f, 0.5f, 1.f))
		, _LineColor_Inactive(FLinearColor(0.15f, 0.15f, 0.15f, 0.7f))
	{}
		SLATE_ARGUMENT(float, LineThickness)
		SLATE_ARGUMENT(FLinearColor, LineColor_Active)
		SLATE_ARGUMENT(FLinearColor, LineColor_Partial)
		SLATE_ARGUMENT(FLinearColor, LineColor_Inactive)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetConnectionData(
		const TArray<TTuple<FGameplayTag, FGameplayTag>>& InPairs,
		const TMap<FGameplayTag, FVector2D>& InPosMap);

	void SetState(
		const FGameplayTagContainer& InAllocated,
		const TArray<FGameplayTag>&  InPending);

	void SetStyle(float InThickness, FLinearColor InActive, FLinearColor InPartial, FLinearColor InInactive);

protected:
	virtual int32 OnPaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override
	{
		return FVector2D::ZeroVector;
	}

private:
	TArray<TTuple<FGameplayTag, FGameplayTag>> Pairs;
	TMap<FGameplayTag, FVector2D>              PosMap;
	FGameplayTagContainer                      AllocatedTags;
	TArray<FGameplayTag>                       PendingTags;
	float                                      Thickness       = 2.f;
	FLinearColor                               Color_Active;
	FLinearColor                               Color_Partial;
	FLinearColor                               Color_Inactive;
};


// ---------------------------------------------------------------------------
// UMG wrapper — placed in the widget BP at ZOrder below Canvas_Nodes
// ---------------------------------------------------------------------------

UCLASS()
class FP_5_7_API UFP_SkillTreeLines : public UWidget
{
	GENERATED_BODY()

public:
	UFP_SkillTreeLines();

	void SetConnectionData(
		const TArray<TTuple<FGameplayTag, FGameplayTag>>& InPairs,
		const TMap<FGameplayTag, FVector2D>& InPosMap);

	void SetState(
		const FGameplayTagContainer& InAllocated,
		const TArray<FGameplayTag>&  InPending);

	void SetStyle(float InThickness, FLinearColor InActive, FLinearColor InPartial, FLinearColor InInactive);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UPROPERTY(EditDefaultsOnly, Category="Lines")
	FLinearColor LineColor_Active = FLinearColor(1.f, 0.8f, 0.2f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category="Lines")
	FLinearColor LineColor_Partial = FLinearColor(0.5f, 0.5f, 0.5f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category="Lines")
	FLinearColor LineColor_Inactive = FLinearColor(0.15f, 0.15f, 0.15f, 0.7f);

	UPROPERTY(EditDefaultsOnly, Category="Lines", meta=(ClampMin="0.5"))
	float LineThickness = 2.0f;

private:
	TSharedPtr<SSkillTreeLines> LinesWidget;

	// Cached data so RebuildWidget can restore state if called after data is set
	TArray<TTuple<FGameplayTag, FGameplayTag>> CachedPairs;
	TMap<FGameplayTag, FVector2D>              CachedPosMap;
	FGameplayTagContainer                      CachedAllocated;
	TArray<FGameplayTag>                       CachedPending;
};

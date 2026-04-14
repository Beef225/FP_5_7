// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_ItemLabelManager.generated.h"

class AFP_ItemActor;
class UFP_ItemPickupWidget;
class UCanvasPanel;

/**
 * Screen-space label manager widget — lives on the HUD canvas.
 *
 * Each in-world AFP_ItemActor registers/unregisters here. Every tick the manager:
 *   1. Culls items that are too far away or behind the camera.
 *   2. Projects remaining item world positions to screen space.
 *   3. Sorts labels by distance (closest = highest priority placement).
 *   4. Runs a vertical push-apart pass to eliminate overlaps.
 *   5. Lerps each label toward its resolved position for smooth camera tracking.
 */
UCLASS()
class FP_5_7_API UFP_ItemLabelManager : public UUserWidget
{
	GENERATED_BODY()

public:

	void RegisterItem(AFP_ItemActor* ItemActor);
	void UnregisterItem(AFP_ItemActor* ItemActor);

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	struct FLabelEntry
	{
		TWeakObjectPtr<AFP_ItemActor> ItemActor;
		TObjectPtr<UFP_ItemPickupWidget> Widget;
		FVector2D CurrentPos{ FVector2D::ZeroVector };
		bool bPositionInitialized{ false };
	};

	void UpdateEntries(APlayerController* PC, APawn* PlayerPawn, float DeltaTime);
	static void ResolveOverlaps(TArray<TPair<FLabelEntry*, FVector2D>>& SortedEntries, const FVector2D& LabelSize, float Padding);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Labels;

	UPROPERTY(EditAnywhere, Category = "Labels")
	TSubclassOf<UFP_ItemPickupWidget> LabelWidgetClass;

	/** World-space distance beyond which labels are hidden. */
	UPROPERTY(EditAnywhere, Category = "Labels")
	float MaxDistance = 1500.f;

	/** Interpolation speed for label position tracking — higher feels snappier. */
	UPROPERTY(EditAnywhere, Category = "Labels")
	float PositionInterpSpeed = 12.f;

	/** Extra gap (in slate units) added between stacked labels during overlap resolution. */
	UPROPERTY(EditAnywhere, Category = "Labels")
	float OverlapPadding = 2.f;

	/**
	 * Approximate rendered size of each label widget in slate units.
	 * Used for overlap detection — should match your WBP_ItemPickupWidget's typical size.
	 */
	UPROPERTY(EditAnywhere, Category = "Labels")
	FVector2D EstimatedLabelSize{ 140.f, 24.f };

	/** Upward offset (slate units) applied to the projected screen position so the label sits above the item. */
	UPROPERTY(EditAnywhere, Category = "Labels")
	float LabelVerticalOffset = 30.f;

	TArray<FLabelEntry> Entries;
};

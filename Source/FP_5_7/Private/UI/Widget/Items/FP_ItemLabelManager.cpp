// Copyright JG

#include "UI/Widget/Items/FP_ItemLabelManager.h"
#include "Actor/Items/FP_ItemActor.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widget/Items/FP_ItemPickupWidget.h"

void UFP_ItemLabelManager::RegisterItem(AFP_ItemActor* ItemActor)
{
	if (!IsValid(ItemActor) || !LabelWidgetClass) return;

	// Prevent duplicates
	for (const FLabelEntry& Entry : Entries)
	{
		if (Entry.ItemActor == ItemActor) return;
	}

	UFP_ItemPickupWidget* Widget = CreateWidget<UFP_ItemPickupWidget>(GetOwningPlayer(), LabelWidgetClass);
	if (!Widget) return;

	Widget->SetItemActor(ItemActor);
	Widget->SetVisibility(ESlateVisibility::Collapsed);

	UCanvasPanelSlot* CanvasSlot = CanvasPanel_Labels->AddChildToCanvas(Widget);
	CanvasSlot->SetAutoSize(true);

	FLabelEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemActor = ItemActor;
	NewEntry.Widget = Widget;
}

void UFP_ItemLabelManager::UnregisterItem(AFP_ItemActor* ItemActor)
{
	for (int32 i = Entries.Num() - 1; i >= 0; --i)
	{
		if (Entries[i].ItemActor == ItemActor)
		{
			Entries[i].Widget->RemoveFromParent();
			Entries.RemoveAtSwap(i);
			return;
		}
	}
}

void UFP_ItemLabelManager::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC)) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!IsValid(PlayerPawn)) return;

	UpdateEntries(PC, PlayerPawn);
}

void UFP_ItemLabelManager::UpdateEntries(APlayerController* PC, APawn* PlayerPawn)
{
	const float MaxDistSq = MaxDistance * MaxDistance;
	const float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());

	TArray<TPair<FLabelEntry*, FVector2D>> VisibleEntries;
	VisibleEntries.Reserve(Entries.Num());

	// Prune stale entries; collect visible ones with their projected target positions
	for (int32 i = Entries.Num() - 1; i >= 0; --i)
	{
		FLabelEntry& Entry = Entries[i];

		if (!Entry.ItemActor.IsValid())
		{
			Entry.Widget->RemoveFromParent();
			Entries.RemoveAtSwap(i);
			continue;
		}

		const FVector ItemWorldPos = Entry.ItemActor->GetActorLocation();
		const float DistSq = FVector::DistSquared(PlayerPawn->GetActorLocation(), ItemWorldPos);

		FVector2D ScreenPos;
		if (!UGameplayStatics::ProjectWorldToScreen(PC, ItemWorldPos, ScreenPos) || DistSq > MaxDistSq)
		{
			Entry.Widget->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}

		// Convert from pixel space to DPI-scaled slate units, center horizontally, apply vertical offset.
		// Use the widget's actual rendered width so centering is accurate regardless of text length or font size.
		// Falls back to EstimatedLabelSize on the first frame before geometry is computed.
		FVector2D TargetPos = ScreenPos / ViewportScale;
		const float ActualWidth = Entry.Widget->GetCachedGeometry().GetLocalSize().X;
		TargetPos.X -= (ActualWidth > 0.f ? ActualWidth : EstimatedLabelSize.X) * 0.5f;
		TargetPos.Y -= LabelVerticalOffset;

		VisibleEntries.Add(TPair<FLabelEntry*, FVector2D>(&Entry, TargetPos));
	}

	if (VisibleEntries.IsEmpty()) return;

	// Sort closest-to-farthest: nearest item wins the unobstructed position
	VisibleEntries.Sort([&](const TPair<FLabelEntry*, FVector2D>& A, const TPair<FLabelEntry*, FVector2D>& B)
	{
		const float DA = FVector::DistSquared(PlayerPawn->GetActorLocation(), A.Key->ItemActor->GetActorLocation());
		const float DB = FVector::DistSquared(PlayerPawn->GetActorLocation(), B.Key->ItemActor->GetActorLocation());
		return DA < DB;
	});

	ResolveOverlaps(VisibleEntries, EstimatedLabelSize, OverlapPadding);

	// Set each label to its resolved position directly — the projected world position already
	// moves smoothly with the camera, so interpolation only adds lag.
	for (auto& [Entry, TargetPos] : VisibleEntries)
	{
		Entry->Widget->SetVisibility(ESlateVisibility::Visible);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Entry->Widget->Slot))
		{
			CanvasSlot->SetPosition(TargetPos);
		}
	}
}

void UFP_ItemLabelManager::ResolveOverlaps(TArray<TPair<FLabelEntry*, FVector2D>>& SortedEntries, const FVector2D& LabelSize, float Padding)
{
	// For each label (starting from second), push upward until it no longer overlaps
	// any higher-priority (closer) label that's already been placed.
	for (int32 i = 1; i < SortedEntries.Num(); ++i)
	{
		FVector2D& Pos = SortedEntries[i].Value;

		for (int32 Iteration = 0; Iteration < 32; ++Iteration)
		{
			bool bAnyOverlap = false;

			for (int32 j = 0; j < i; ++j)
			{
				const FVector2D& PlacedPos = SortedEntries[j].Value;
				const float DX = FMath::Abs(Pos.X - PlacedPos.X);
				const float DY = FMath::Abs(Pos.Y - PlacedPos.Y);

				if (DX < LabelSize.X + Padding && DY < LabelSize.Y + Padding)
				{
					// Push this label above the one it overlaps
					Pos.Y = PlacedPos.Y - LabelSize.Y - Padding;
					bAnyOverlap = true;
					break;
				}
			}

			if (!bAnyOverlap) break;
		}
	}
}

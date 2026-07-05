// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_InteractionPromptWidget.generated.h"

class UButton;
class UTextBlock;

/**
 * World-space prompt shown above an interactable (loot container, level transition, ...)
 * while it's highlighted. The button triggers the exact same interaction the player gets
 * from clicking the actor directly in the world, via AFP_PlayerController::RequestInteraction
 * — this is an alternate affordance/readout, not a separate system; it doesn't touch the
 * inventory/item UI at all.
 *
 * Usage: create a Blueprint subclass with a Button named Button_Interact and a TextBlock
 * named Text_Prompt (BindWidget requires exact names), style it however you like (black
 * background, etc.), then assign that Blueprint as the WidgetClass on a UWidgetComponent
 * (World space) on your interactable actor.
 */
UCLASS()
class FP_5_7_API UFP_InteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Called by the owning actor once the widget exists so it knows who to interact with and what label to show. */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void InitPrompt(AActor* InTargetActor, const FText& InPromptText);

protected:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void OnPromptButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Interact;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Prompt;

	TWeakObjectPtr<AActor> TargetActor;
};

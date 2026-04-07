// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_ItemPickupWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class AFP_ItemActor;

/**
 * World-space hover widget attached to AFP_ItemActor via UWidgetComponent.
 * Shows the item name and an interact button the player presses to pick up.
 */
UCLASS()
class FP_5_7_API UFP_ItemPickupWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Called by the owning ItemActor after it spawns so the widget knows who to talk to. */
	void SetItemActor(AFP_ItemActor* InItemActor);

protected:

	virtual void NativeOnInitialized() override;

	/** Override in Blueprint to play show/hide animations. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnPickupWidgetShown();

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnPickupWidgetHidden();

private:

	UFUNCTION()
	void OnPickupButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Pickup;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Background;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ItemName;

	UPROPERTY(EditAnywhere, Category = "Item", meta = (AllowPrivateAccess = "true"))
	FSlateFontInfo ItemNameFont;

	TWeakObjectPtr<AFP_ItemActor> ItemActor;
};

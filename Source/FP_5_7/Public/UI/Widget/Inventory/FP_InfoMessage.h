// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_InfoMessage.generated.h"

class UTextBlock;

/**
 * A transient notification widget. Call SetMessage() to display text for MessageLifetime seconds.
 * Implement MessageShow / MessageHide in Blueprint for enter/exit animations.
 */
UCLASS()
class FP_5_7_API UFP_InfoMessage : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	void SetMessage(const FText& Message);

	UFUNCTION(BlueprintImplementableEvent, Category = "InfoMessage")
	void MessageShow();

	UFUNCTION(BlueprintImplementableEvent, Category = "InfoMessage")
	void MessageHide();

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Message;

	UPROPERTY(EditAnywhere, Category = "InfoMessage")
	float MessageLifetime = 3.f;

	FTimerHandle MessageTimer;
	bool bIsMessageActive = false;
};

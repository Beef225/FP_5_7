// Copyright JG

#include "UI/Widget/Inventory/FP_InfoMessage.h"
#include "Components/TextBlock.h"

void UFP_InfoMessage::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Text_Message->SetText(FText::GetEmpty());
	MessageHide();
}

void UFP_InfoMessage::SetMessage(const FText& Message)
{
	Text_Message->SetText(Message);

	if (!bIsMessageActive)
	{
		MessageShow();
	}
	bIsMessageActive = true;

	GetWorld()->GetTimerManager().SetTimer(MessageTimer, [this]()
	{
		MessageHide();
		bIsMessageActive = false;
	}, MessageLifetime, false);
}

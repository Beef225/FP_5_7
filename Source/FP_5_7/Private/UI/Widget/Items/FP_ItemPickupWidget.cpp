// Copyright JG

#include "UI/Widget/Items/FP_ItemPickupWidget.h"
#include "Actor/Items/FP_ItemActor.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UFP_ItemPickupWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_Pickup)
	{
		Button_Pickup->OnClicked.AddDynamic(this, &ThisClass::OnPickupButtonClicked);
	}
}

void UFP_ItemPickupWidget::SetItemActor(AFP_ItemActor* InItemActor)
{
	ItemActor = InItemActor;
}

void UFP_ItemPickupWidget::OnPickupButtonClicked()
{
	if (ItemActor.IsValid())
	{
		ItemActor->OnPickupRequested();
	}
}

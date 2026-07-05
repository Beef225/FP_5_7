// Copyright JG

#include "UI/Widget/World/FP_InteractionPromptWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/FP_PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UFP_InteractionPromptWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_Interact)
	{
		Button_Interact->OnClicked.AddDynamic(this, &ThisClass::OnPromptButtonClicked);
	}
}

void UFP_InteractionPromptWidget::InitPrompt(AActor* InTargetActor, const FText& InPromptText)
{
	TargetActor = InTargetActor;

	if (Text_Prompt)
	{
		Text_Prompt->SetText(InPromptText);
	}
}

void UFP_InteractionPromptWidget::OnPromptButtonClicked()
{
	if (!TargetActor.IsValid()) return;

	if (AFP_PlayerController* PC = Cast<AFP_PlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->RequestInteraction(TargetActor.Get());
	}
}

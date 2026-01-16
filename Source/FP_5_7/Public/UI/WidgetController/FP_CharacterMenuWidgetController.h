// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_WidgetController.h"
#include "FP_CharacterMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_CharacterMenuWidgetController : public UFP_WidgetController
{
	GENERATED_BODY()
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
};

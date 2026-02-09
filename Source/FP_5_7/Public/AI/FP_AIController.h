// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FP_AIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;

UCLASS()
class FP_5_7_API AFP_AIController : public AAIController
{
	GENERATED_BODY()

public:

	AFP_AIController();

protected:

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};

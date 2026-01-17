// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_WidgetController.h"
#include "FP_CharacterMenuWidgetController.generated.h"

class UFP_AttributeInfo;
struct FAttributeInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAttributeInfo&, Info);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FP_5_7_API UFP_CharacterMenuWidgetController : public UFP_WidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
	
	UPROPERTY(BlueprintAssignable, Category="GAS| Attributues")
	FAttributeInfoSignature AttributeInfoDelegate;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFP_AttributeInfo> AttributeInfo;
	
private:
	void BroadcastAttributeInfo (const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};

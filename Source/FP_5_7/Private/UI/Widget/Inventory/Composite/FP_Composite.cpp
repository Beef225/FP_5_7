// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_Composite.h"
#include "Blueprint/WidgetTree.h"

void UFP_Composite::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		if (UFP_CompositeBase* Composite = Cast<UFP_CompositeBase>(Widget); IsValid(Composite))
		{
			Children.Add(Composite);
			Composite->Collapse();
		}
	});
}

void UFP_Composite::ApplyFunction(FuncType Function)
{
	for (auto& Child : Children)
	{
		Child->ApplyFunction(Function);
	}
}

void UFP_Composite::Collapse()
{
	for (auto& Child : Children)
	{
		Child->Collapse();
	}
}

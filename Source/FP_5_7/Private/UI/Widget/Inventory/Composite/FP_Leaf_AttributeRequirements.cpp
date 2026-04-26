// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_Leaf_AttributeRequirements.h"
#include "Components/RichTextBlock.h"

void UFP_Leaf_AttributeRequirements::SetRequirements(
	FStatRequirement Might,
	FStatRequirement Resonance,
	FStatRequirement Agility,
	FStatRequirement Fortitude)
{
	if (!IsValid(Text_Requirements)) return;

	TArray<FString> Parts;

	auto AddPart = [&](const FStatRequirement& Req, const TCHAR* Name)
	{
		if (Req.Required == 0) return;
		if (Req.Current >= Req.Required)
			Parts.Add(FString::Printf(TEXT("%d %s"), Req.Required, Name));
		else
			Parts.Add(FString::Printf(TEXT("<fail>%d %s</>"), Req.Required, Name));
	};

	AddPart(Might,     TEXT("MGHT"));
	AddPart(Resonance, TEXT("RESO"));
	AddPart(Agility,   TEXT("AGIL"));
	AddPart(Fortitude, TEXT("FORT"));

	if (Parts.IsEmpty()) return;

	const FString Line = TEXT("REQUIRES ") + FString::Join(Parts, TEXT(", "));
	Text_Requirements->SetText(FText::FromString(Line));
}

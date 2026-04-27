// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_Leaf_AttributeRequirements.h"
#include "Components/RichTextBlock.h"

void UFP_Leaf_AttributeRequirements::SetRequirements(
	FStatRequirement Level,
	FStatRequirement Might,
	FStatRequirement Resonance,
	FStatRequirement Agility,
	FStatRequirement Fortitude)
{
	if (!IsValid(Text_Requirements)) return;

	TArray<FString> Parts;

	if (Level.Required > 0)
	{
		const FString LvlStr = FString::Printf(TEXT("LVL. %d"), Level.Required);
		Parts.Add(Level.Current >= Level.Required
			? LvlStr
			: FString::Printf(TEXT("<fail>%s</>"), *LvlStr));
	}

	auto AddPart = [&](const FStatRequirement& Req, const TCHAR* Name)
	{
		if (Req.Required == 0) return;
		Parts.Add(Req.Current >= Req.Required
			? FString::Printf(TEXT("%d %s"), Req.Required, Name)
			: FString::Printf(TEXT("<fail>%d %s</>"), Req.Required, Name));
	};

	AddPart(Might,     TEXT("MGHT"));
	AddPart(Resonance, TEXT("RESO"));
	AddPart(Agility,   TEXT("AGIL"));
	AddPart(Fortitude, TEXT("FORT"));

	if (Parts.IsEmpty()) return;

	const FString Line = TEXT("REQUIRES ") + FString::Join(Parts, TEXT(", "));
	Text_Requirements->SetText(FText::FromString(Line));
}

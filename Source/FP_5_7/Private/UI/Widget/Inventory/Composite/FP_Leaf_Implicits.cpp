// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_Leaf_Implicits.h"
#include "Components/RichTextBlock.h"

static FString FormatImplicitStatValue(float Value, int32 DecimalCount, EFP_AffixDisplayAs DisplayAs)
{
	if (DisplayAs == EFP_AffixDisplayAs::Percent)
		Value *= 100.f;
	FNumberFormattingOptions Opts;
	Opts.MinimumFractionalDigits = DecimalCount;
	Opts.MaximumFractionalDigits = DecimalCount;
	return FText::AsNumber(Value, &Opts).ToString();
}

static FString BuildImplicitLine(const FFP_AffixInstance& Implicit)
{
	FString Line = Implicit.DisplayFormat;
	Line.ReplaceInline(
		TEXT("{1}"),
		*FormatImplicitStatValue(Implicit.Stat1_Value, Implicit.Stat1_DecimalCount, Implicit.Stat1_DisplayAs));
	if (Implicit.HasStat2())
	{
		Line.ReplaceInline(
			TEXT("{2}"),
			*FormatImplicitStatValue(Implicit.Stat2_Value, Implicit.Stat2_DecimalCount, Implicit.Stat2_DisplayAs));
	}
	return Line;
}

void UFP_Leaf_Implicits::SetImplicits(const TArray<FFP_AffixInstance>& Implicits)
{
	FString AllLines;
	for (int32 i = 0; i < Implicits.Num(); ++i)
	{
		if (i > 0) AllLines += TEXT("\n");
		AllLines += BuildImplicitLine(Implicits[i]);
	}
	RichText_Implicits->SetText(FText::FromString(AllLines));
}

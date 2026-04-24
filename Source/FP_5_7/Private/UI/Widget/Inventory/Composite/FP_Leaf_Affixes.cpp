// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_Leaf_Affixes.h"
#include "Components/RichTextBlock.h"

static FString FormatStatValue(float Value, int32 DecimalCount, EFP_AffixDisplayAs /*DisplayAs*/)
{
	FNumberFormattingOptions Opts;
	Opts.MinimumFractionalDigits = DecimalCount;
	Opts.MaximumFractionalDigits = DecimalCount;
	return FText::AsNumber(Value, &Opts).ToString();
}

static FString BuildAffixLine(const FFP_AffixInstance& Affix)
{
	FString Line = Affix.DisplayFormat;
	Line.ReplaceInline(
		TEXT("{1}"),
		*FormatStatValue(Affix.Stat1_Value, Affix.Stat1_DecimalCount, Affix.Stat1_DisplayAs));
	if (Affix.HasStat2())
	{
		Line.ReplaceInline(
			TEXT("{2}"),
			*FormatStatValue(Affix.Stat2_Value, Affix.Stat2_DecimalCount, Affix.Stat2_DisplayAs));
	}
	return Line;
}

void UFP_Leaf_Affixes::SetAffixes(const TArray<FFP_AffixInstance>& SortedAffixes)
{
	FString AllLines;
	for (int32 i = 0; i < SortedAffixes.Num(); ++i)
	{
		if (i > 0) AllLines += TEXT("\n");
		AllLines += BuildAffixLine(SortedAffixes[i]);
	}
	RichText_Affixes->SetText(FText::FromString(AllLines));
}

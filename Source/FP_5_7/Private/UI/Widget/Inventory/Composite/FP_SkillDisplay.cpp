// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillDisplay.h"
#include "AbilitySystem/Data/FP_SkillLevelUpInfo.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UFP_SkillDisplay::Populate(const FFP_AbilityEntry& Entry, int32 CurrentXP)
{
	if (Entry.SkillIcon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(const_cast<UTexture2D*>(Entry.SkillIcon.Get()));
		Brush.DrawAs = ESlateBrushDrawType::Image;
		Image_SkillIcon->SetBrush(Brush);
	}

	RichText_SkillName->SetText(FText::FromName(Entry.SkillName));

	const UFP_SkillLevelUpInfo* Curve = Entry.SkillLevelUpInfo;
	if (Curve)
	{
		const int32 MaxLevel = FMath::Min(Entry.MaxLevel, Curve->GetMaxLevel());
		const int32 CurrentLevel = FMath::Min(Curve->FindLevelForXP(CurrentXP), MaxLevel);

		Text_SkillLevel->SetText(FText::FromString(FString::Printf(TEXT("LVL %d"), CurrentLevel)));

		float Percent = 1.f;
		if (CurrentLevel < MaxLevel)
		{
			const int32 ThisReq = Curve->GetXPRequirementForLevel(CurrentLevel);
			const int32 NextReq = Curve->GetXPRequirementForLevel(CurrentLevel + 1);
			const int32 Delta = NextReq - ThisReq;
			Percent = (Delta > 0)
				? FMath::Clamp(static_cast<float>(CurrentXP - ThisReq) / static_cast<float>(Delta), 0.f, 1.f)
				: 1.f;
		}

		ProgressBar_SkillXP->SetPercent(Percent);
		ProgressBar_SkillXP->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Text_SkillLevel->SetText(FText::FromString(TEXT("LVL 1")));
		ProgressBar_SkillXP->SetVisibility(ESlateVisibility::Collapsed);
	}
}

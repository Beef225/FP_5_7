// Copyright JG

#include "UI/Widget/HUD/FP_SkillFrame.h"
#include "UI/Widget/HUD/FP_SkillFrameActions.h"
#include "UI/Widget/HUD/FP_SkillPickerEntry.h"
#include "UI/Widget/HUD/FP_SkillPickerPopup.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Engine/Texture2D.h"
#include "EnhancedActionKeyMapping.h"
#include "GameFramework/PlayerController.h"
#include "Input/FP_InputConfig.h"
#include "Input/FP_KeyIconSet.h"
#include "InputMappingContext.h"
#include "Player/FP_PlayerState.h"

void UFP_SkillFrame::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_SkillList)
		Button_SkillList->OnClicked.AddDynamic(this, &UFP_SkillFrame::OnSkillListButtonClicked);

	RefreshKeyIcon();

	if (AFP_PlayerState* PS = GetFPPlayerState())
		SkillAssignedHandle = PS->OnSkillInputTagAssigned.AddUObject(this, &UFP_SkillFrame::HandleSkillInputTagAssigned);

	if (UFP_AbilitySystemComponent* ASC = GetFPASC())
	{
		if (ASC->bStartupAbilitiesGiven)
		{
			RestoreFromPlayerState();
			RefreshIcon();
		}
		else
		{
			// Widget constructed before abilities granted — refresh once they are
			ASC->AbilitiesGivenDelegate.AddUObject(this, &UFP_SkillFrame::OnAbilitiesGiven);
		}
	}
}

void UFP_SkillFrame::NativeDestruct()
{
	ClosePickerPopup();

	if (AFP_PlayerState* PS = GetFPPlayerState())
		PS->OnSkillInputTagAssigned.Remove(SkillAssignedHandle);

	Super::NativeDestruct();
}

void UFP_SkillFrame::OnSkillListButtonClicked()
{
	if (ActivePickerPopup.IsValid())
	{
		ClosePickerPopup();
		return;
	}
	PopulateSkillPicker();
}

void UFP_SkillFrame::PopulateSkillPicker()
{
	if (!SkillPickerPopupClass || !SkillPickerEntryClass) return;

	AFP_PlayerState* PS = GetFPPlayerState();
	if (!PS) return;

	const UFP_SkillLibrary* Library = PS->GetSkillLibrary();
	if (!Library) return;

	UFP_SkillPickerPopup* Popup = CreateWidget<UFP_SkillPickerPopup>(GetOwningPlayer(), SkillPickerPopupClass);
	if (!Popup) return;

	UScrollBox* ScrollBox = Popup->GetScrollBox();
	if (!ScrollBox) return;

	// Actions bar at the top
	if (SkillFrameActionsClass)
	{
		UFP_SkillFrameActions* ActionsWidget = CreateWidget<UFP_SkillFrameActions>(GetOwningPlayer(), SkillFrameActionsClass);
		if (ActionsWidget)
		{
			ActionsWidget->Init(this);
			ScrollBox->AddChild(ActionsWidget);
		}
	}

	const bool bIsInjector = (BarType == ESkillBarType::Injector);

	for (const FFP_AbilityEntry& Entry : Library->AbilityEntries)
	{
		if (!Entry.bShowInSkillMenu) continue;
		if (bIsInjector ? !Entry.bShowInInjectorBar : !Entry.bShowInSkillBar) continue;
		if (!PS->IsSkillGranted(Entry.SkillTag)) continue;

		UFP_SkillPickerEntry* EntryWidget = CreateWidget<UFP_SkillPickerEntry>(GetOwningPlayer(), SkillPickerEntryClass);
		if (!EntryWidget) continue;

		EntryWidget->Populate(Entry, PS->GetSkillXP(Entry.SkillTag), this);
		ScrollBox->AddChild(EntryWidget);
	}

	// Bottom of popup aligns to top of this frame
	const FGeometry& Geo = GetCachedGeometry();
	const FVector2D FrameTopLeft = Geo.GetAbsolutePosition();

	Popup->AddToViewport(10);
	Popup->SetPositionInViewport(FrameTopLeft, /*bRemoveDPIScale=*/false);
	Popup->SetAlignmentInViewport(FVector2D(0.f, 1.f)); // anchor: bottom-left of popup → FrameTopLeft

	ActivePickerPopup = Popup;
}

void UFP_SkillFrame::ClosePickerPopup()
{
	if (UFP_SkillPickerPopup* Popup = ActivePickerPopup.Get())
		Popup->RemoveFromParent();
	ActivePickerPopup.Reset();
}

void UFP_SkillFrame::AssignSkill(const FGameplayTag& SkillTag)
{
	AFP_PlayerState* PS = GetFPPlayerState();
	UFP_AbilitySystemComponent* ASC = GetFPASC();
	if (!PS || !ASC || !SlotInputTag.IsValid()) return;

	ASC->AssignInputTagToSkill(SkillTag, SlotInputTag);
	PS->AssignSkillToSlot(SkillTag, SlotInputTag);

	AssignedSkillTag = SkillTag;
	RefreshIcon();
	ClosePickerPopup();
}

void UFP_SkillFrame::ClearSlot()
{
	if (AssignedSkillTag.IsValid())
	{
		if (UFP_AbilitySystemComponent* ASC = GetFPASC())
			ASC->AssignInputTagToSkill(AssignedSkillTag, FGameplayTag());

		if (AFP_PlayerState* PS = GetFPPlayerState())
			PS->ClearSkillSlot(SlotInputTag);

		AssignedSkillTag = FGameplayTag();
		RefreshIcon();
	}

	ClosePickerPopup();
}

void UFP_SkillFrame::BeginMoveSlot()
{
	// Move-to-another-slot flow — deferred
}

void UFP_SkillFrame::RefreshIcon()
{
	if (!Image_SkillIcon) return;

	if (!AssignedSkillTag.IsValid())
	{
		if (DefaultIcon)
			Image_SkillIcon->SetBrushFromTexture(DefaultIcon);
		else
			Image_SkillIcon->SetBrush(FSlateBrush());
		return;
	}

	const AFP_PlayerState* PS = GetFPPlayerState();
	if (!PS) return;

	const UFP_SkillLibrary* Library = PS->GetSkillLibrary();
	if (!Library) return;

	const FFP_AbilityEntry Entry = Library->FindAbilityEntryForTag(AssignedSkillTag);
	if (!Entry.SkillTag.IsValid()) return;

	if (Entry.SkillIcon)
		Image_SkillIcon->SetBrushFromTexture(const_cast<UTexture2D*>(Entry.SkillIcon.Get()));
	else
		Image_SkillIcon->SetBrush(FSlateBrush());
}

void UFP_SkillFrame::RefreshKeyIcon()
{
	if (!Image_KeyIcon) return;

	auto ClearIcon = [this]() { Image_KeyIcon->SetBrush(FSlateBrush()); };

	if (!InputConfig || !MappingContext || !KeyIconSet || !SlotInputTag.IsValid())
	{
		ClearIcon();
		return;
	}

	// Step 1: InputTag → UInputAction
	const UInputAction* Action = InputConfig->FindAbilityInputActionForTag(SlotInputTag);
	if (!Action) { ClearIcon(); return; }

	// Step 2: UInputAction → FKey (first binding found in the IMC)
	FKey BoundKey;
	for (const FEnhancedActionKeyMapping& Mapping : MappingContext->GetMappings())
	{
		if (Mapping.Action == Action)
		{
			BoundKey = Mapping.Key;
			break;
		}
	}
	if (!BoundKey.IsValid()) { ClearIcon(); return; }

	// Step 3: FKey → icon texture
	UTexture2D* Icon = KeyIconSet->GetIconForKey(BoundKey);
	if (!Icon) { ClearIcon(); return; }

	Image_KeyIcon->SetBrushFromTexture(Icon);
}

void UFP_SkillFrame::RestoreFromPlayerState()
{
	AssignedSkillTag = FGameplayTag();
	if (!SlotInputTag.IsValid()) return;

	AFP_PlayerState* PS = GetFPPlayerState();

	// Primary: PlayerState map covers runtime assignments and loaded saves
	if (PS)
	{
		for (const auto& Pair : PS->GetSkillInputTagMap())
		{
			if (Pair.Value.MatchesTagExact(SlotInputTag))
			{
				AssignedSkillTag = Pair.Key;
				return;
			}
		}
	}

	// Fallback: scan ASC specs for startup abilities whose StartupInputTag matches this slot.
	// AddCharacterAbilities sets the tag directly on the spec, bypassing AssignSkillToSlot.
	UFP_AbilitySystemComponent* ASC = GetFPASC();
	if (!ASC) return;

	FForEachAbilty ScanDelegate;
	ScanDelegate.BindLambda([this, ASC](const FGameplayAbilitySpec& Spec)
	{
		if (AssignedSkillTag.IsValid()) return;
		if (ASC->GetInputTagFromSpec(Spec).MatchesTagExact(SlotInputTag))
		{
			const FGameplayTag Found = ASC->GetAbilityTagFromSpec(Spec);
			if (Found.IsValid())
				AssignedSkillTag = Found;
		}
	});
	ASC->ForEachAbility(ScanDelegate);

	// Sync the PS map so this startup assignment persists in saves going forward
	if (AssignedSkillTag.IsValid() && PS)
		PS->SetSkillInputTag(AssignedSkillTag, SlotInputTag);
}

void UFP_SkillFrame::OnAbilitiesGiven(UFP_AbilitySystemComponent* /*ASC*/)
{
	RestoreFromPlayerState();
	RefreshIcon();
}

void UFP_SkillFrame::HandleSkillInputTagAssigned(FGameplayTag SkillTag, FGameplayTag NewInputTag)
{
	// Another frame stole our assigned skill — clear this slot
	if (SkillTag.MatchesTagExact(AssignedSkillTag) && !NewInputTag.MatchesTagExact(SlotInputTag))
	{
		AssignedSkillTag = FGameplayTag();
		RefreshIcon();
	}
}

AFP_PlayerState* UFP_SkillFrame::GetFPPlayerState() const
{
	if (const APlayerController* PC = GetOwningPlayer())
		return PC->GetPlayerState<AFP_PlayerState>();
	return nullptr;
}

UFP_AbilitySystemComponent* UFP_SkillFrame::GetFPASC() const
{
	if (const AFP_PlayerState* PS = GetFPPlayerState())
		return Cast<UFP_AbilitySystemComponent>(PS->GetAbilitySystemComponent());
	return nullptr;
}

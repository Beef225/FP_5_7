// Copyright JG

#include "UI/Widget/HUD/FP_SkillFrame.h"
#include "UI/Widget/HUD/FP_SkillFrameActions.h"
#include "UI/Widget/HUD/FP_SkillPickerEntry.h"
#include "UI/Widget/HUD/FP_SkillPickerPopup.h"
#include "UI/Widget/Inventory/Composite/FP_SkillDescription.h"
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

static TWeakObjectPtr<UFP_SkillFrame> GActivePickerFrame;

void UFP_SkillFrame::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_SkillList)
		Button_SkillList->OnClicked.AddDynamic(this, &UFP_SkillFrame::OnSkillListButtonClicked);

	RefreshKeyIcon();

	if (AFP_PlayerState* PS = GetFPPlayerState())
	{
		SkillAssignedHandle    = PS->OnSkillInputTagAssigned.AddUObject(this, &UFP_SkillFrame::HandleSkillInputTagAssigned);
		SkillStateLoadedHandle = PS->OnSkillStateLoaded.AddUObject(this, &UFP_SkillFrame::OnSkillStateLoaded);
	}

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

void UFP_SkillFrame::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	OpenDescriptionPopup();
}

void UFP_SkillFrame::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	CloseDescriptionPopup();
}

void UFP_SkillFrame::NativeDestruct()
{
	CloseDescriptionPopup();
	ClosePickerPopup();

	if (AFP_PlayerState* PS = GetFPPlayerState())
	{
		PS->OnSkillInputTagAssigned.Remove(SkillAssignedHandle);
		PS->OnSkillStateLoaded.Remove(SkillStateLoadedHandle);
	}

	Super::NativeDestruct();
}

void UFP_SkillFrame::OnSkillListButtonClicked()
{
	CloseDescriptionPopup();

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

	// Close any other frame's picker first
	if (UFP_SkillFrame* Other = GActivePickerFrame.Get())
	{
		if (Other != this)
			Other->ClosePickerPopup();
	}

	UFP_SkillPickerPopup* Popup = CreateWidget<UFP_SkillPickerPopup>(GetOwningPlayer(), SkillPickerPopupClass);
	if (!Popup) return;

	Popup->Init(this);

	// Stretch popup to fill the viewport so Button_Backdrop catches all off-content clicks
	Popup->AddToViewport(10);
	Popup->SetAnchorsInViewport(FAnchors(0.f, 0.f, 1.f, 1.f));

	// Position Border_Content so its bottom-centre aligns to the top-centre of this frame
	const FGeometry& Geo = GetCachedGeometry();
	const FVector2D FrameTopLeft = Geo.GetAbsolutePosition();
	const FVector2D FrameSize    = Geo.GetAbsoluteSize();
	Popup->PositionContent(FVector2D(FrameTopLeft.X + FrameSize.X * 0.5f, FrameTopLeft.Y));

	ActivePickerPopup = Popup;
	GActivePickerFrame = this;

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
}

void UFP_SkillFrame::OpenDescriptionPopup()
{
	if (!SkillDescriptionClass || !AssignedSkillTag.IsValid()) return;
	if (ActiveDescriptionPopup.IsValid()) return;

	AFP_PlayerState* PS = GetFPPlayerState();
	if (!PS) return;

	const UFP_SkillLibrary* Library = PS->GetSkillLibrary();
	if (!Library) return;

	const FFP_AbilityEntry Entry = Library->FindAbilityEntryForTag(AssignedSkillTag);
	if (!Entry.SkillTag.IsValid()) return;

	UFP_SkillDescription* Desc = CreateWidget<UFP_SkillDescription>(GetOwningPlayer(), SkillDescriptionClass);
	if (!Desc) return;

	Desc->Populate(Entry, PS);

	const FGeometry& Geo     = GetCachedGeometry();
	const FVector2D TopLeft  = Geo.GetAbsolutePosition();
	const FVector2D Size     = Geo.GetAbsoluteSize();
	const FVector2D Anchor   = FVector2D(TopLeft.X + Size.X * 0.5f, TopLeft.Y);

	Desc->AddToViewport(9); // below picker at 10
	Desc->SetPositionInViewport(Anchor, /*bRemoveDPIScale=*/true);
	Desc->SetAlignmentInViewport(FVector2D(0.5f, 1.f)); // bottom-centre of description → top-centre of frame

	ActiveDescriptionPopup = Desc;
}

void UFP_SkillFrame::CloseDescriptionPopup()
{
	if (UFP_SkillDescription* Desc = ActiveDescriptionPopup.Get())
		Desc->RemoveFromParent();
	ActiveDescriptionPopup.Reset();
}

void UFP_SkillFrame::ClosePickerPopup()
{
	if (UFP_SkillPickerPopup* Popup = ActivePickerPopup.Get())
		Popup->RemoveFromParent();
	ActivePickerPopup.Reset();

	if (GActivePickerFrame.Get() == this)
		GActivePickerFrame.Reset();
}

void UFP_SkillFrame::AssignSkill(const FGameplayTag& SkillTag)
{
	AFP_PlayerState* PS = GetFPPlayerState();
	UFP_AbilitySystemComponent* ASC = GetFPASC();
	if (!PS || !ASC || !SlotInputTag.IsValid()) return;

	// Remove old slot occupant from ASC before assigning new skill
	const FGameplayTag OldSkill = PS->GetSkillForSlot(SlotInputTag);
	if (OldSkill.IsValid() && !OldSkill.MatchesTagExact(SkillTag))
		ASC->RemoveInputTagFromSkill(OldSkill, SlotInputTag);

	ASC->AddInputTagToSkill(SkillTag, SlotInputTag);
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
			ASC->RemoveInputTagFromSkill(AssignedSkillTag, SlotInputTag);

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

	// Primary: PlayerState map covers runtime assignments and loaded saves (slot is the key)
	if (PS)
	{
		const FGameplayTag Found = PS->GetSkillForSlot(SlotInputTag);
		if (Found.IsValid())
		{
			AssignedSkillTag = Found;
			return;
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
		PS->SetSkillSlot(SlotInputTag, AssignedSkillTag);
}

void UFP_SkillFrame::OnAbilitiesGiven(UFP_AbilitySystemComponent* /*ASC*/)
{
	RestoreFromPlayerState();
	RefreshIcon();
}

void UFP_SkillFrame::OnSkillStateLoaded()
{
	RestoreFromPlayerState();
	RefreshIcon();
}

void UFP_SkillFrame::HandleSkillInputTagAssigned(FGameplayTag InSlotInputTag, FGameplayTag InSkillTag)
{
	// Only react to events for this frame's slot
	if (!InSlotInputTag.MatchesTagExact(SlotInputTag)) return;

	AssignedSkillTag = InSkillTag; // invalid means slot was cleared
	RefreshIcon();
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

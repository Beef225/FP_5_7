// Copyright JG

#include "Editor/FP_ItemActorDetails.h"

#if WITH_EDITOR

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Actor/Items/FP_ItemActor.h"

#define LOCTEXT_NAMESPACE "FP_ItemActorDetails"

// ---------------------------------------------------------------------------
// Detail customization
// ---------------------------------------------------------------------------

TSharedRef<IDetailCustomization> FFP_ItemActorDetails::MakeInstance()
{
	return MakeShared<FFP_ItemActorDetails>();
}

void FFP_ItemActorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);
	if (Objects.IsEmpty()) return;

	AFP_ItemActor* Actor = Cast<AFP_ItemActor>(Objects[0].Get());
	if (!Actor) return;

	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(
		"Item|Setup", FText::GetEmpty(), ECategoryPriority::Important);

	Category.AddCustomRow(LOCTEXT("ApplyTemplateRow", "Apply Fragment Template"))
	.WholeRowContent()
	[
		SNew(SButton)
		.Text(LOCTEXT("ApplyTemplateBtn", "Apply Fragment Template"))
		.HAlign(HAlign_Center)
		.OnClicked_Lambda([Actor]()
		{
			if (IsValid(Actor))
				Actor->ApplyFragmentTemplate();
			return FReply::Handled();
		})
	];
}

// ---------------------------------------------------------------------------
// Self-registrar — no custom module class needed, avoids IMPLEMENT_MODULE
// conflicts with UBT-generated unity batch files.
// ---------------------------------------------------------------------------

struct FFP_ItemActorDetailsRegistrar
{
	FFP_ItemActorDetailsRegistrar()
	{
		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			Register();
		}
		else
		{
			OnChangedHandle = FModuleManager::Get().OnModulesChanged().AddRaw(
				this, &FFP_ItemActorDetailsRegistrar::OnModulesChanged);
		}
	}

	~FFP_ItemActorDetailsRegistrar()
	{
		FModuleManager::Get().OnModulesChanged().Remove(OnChangedHandle);

		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor")
				.UnregisterCustomClassLayout(FName("FP_ItemActor"));
		}
	}

	void OnModulesChanged(FName ModuleName, EModuleChangeReason Reason)
	{
		if (ModuleName == "PropertyEditor" && Reason == EModuleChangeReason::ModuleLoaded)
		{
			Register();
			FModuleManager::Get().OnModulesChanged().Remove(OnChangedHandle);
		}
	}

	void Register()
	{
		FPropertyEditorModule& PropertyModule =
			FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout(
			FName("FP_ItemActor"),
			FOnGetDetailCustomizationInstance::CreateStatic(&FFP_ItemActorDetails::MakeInstance));
		PropertyModule.NotifyCustomizationModuleChanged();
	}

	FDelegateHandle OnChangedHandle;
};

// Constructed when the FP_5_7 DLL loads — no module class or startup hook required.
static FFP_ItemActorDetailsRegistrar GItemActorDetailsRegistrar;

#undef LOCTEXT_NAMESPACE

#endif // WITH_EDITOR

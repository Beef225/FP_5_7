// Copyright JG

#include "Preview/FP_CharacterPreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/IInputProcessor.h"
#include "InputCoreTypes.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Inventory/Items/Fragments/FP_GripStanceFragment.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "Kismet/GameplayStatics.h"
#include "SaveSystem/FP_InventorySaveData.h"

// ---------------------------------------------------------------------------
// Input processor — intercepts raw mouse button events from Slate before any
// widget can consume them, so drag-to-rotate works even in UIOnly input mode.
// ---------------------------------------------------------------------------

class FFP_PreviewInputProcessor : public IInputProcessor
{
public:
	bool bIsLeftMouseDown = false;

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
			bIsLeftMouseDown = true;
		return false; // never consume — let UMG handle the event too
	}

	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
			bIsLeftMouseDown = false;
		return false;
	}

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {}
};

// ---------------------------------------------------------------------------

AFP_CharacterPreviewActor::AFP_CharacterPreviewActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh_Body drives animation; all other parts follow it via SetLeaderPoseComponent (done in BP BeginPlay).
	Mesh_Body  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Body"));
	SetRootComponent(Mesh_Body);

	Mesh_Head  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Head"));
	Mesh_Head->SetupAttachment(Mesh_Body);

	Mesh_Torso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Torso"));
	Mesh_Torso->SetupAttachment(Mesh_Body);

	Mesh_Arms  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Arms"));
	Mesh_Arms->SetupAttachment(Mesh_Body);

	Mesh_Hands = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Hands"));
	Mesh_Hands->SetupAttachment(Mesh_Body);

	Mesh_Legs  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Legs"));
	Mesh_Legs->SetupAttachment(Mesh_Body);

	Mesh_Feet  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Feet"));
	Mesh_Feet->SetupAttachment(Mesh_Body);
}

void AFP_CharacterPreviewActor::BeginPlay()
{
	Super::BeginPlay();
	ResetToDefault();
	SetActorHiddenInGame(true);

	if (FSlateApplication::IsInitialized())
	{
		InputProcessor = MakeShared<FFP_PreviewInputProcessor>();
		FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor);
	}
}

void AFP_CharacterPreviewActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (FSlateApplication::IsInitialized() && InputProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
		InputProcessor.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void AFP_CharacterPreviewActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!InputProcessor.IsValid() || !FSlateApplication::IsInitialized()) return;

	const bool bMouseDown     = InputProcessor->bIsLeftMouseDown;
	const FVector2D CursorPos = FSlateApplication::Get().GetCursorPos();

	if (bMouseDown && bWasMouseDown)
	{
		const float DeltaX = CursorPos.X - LastMouseX;
		AddActorLocalRotation(FRotator(0.f, -DeltaX * RotationSpeed, 0.f));
	}

	bWasMouseDown = bMouseDown;
	LastMouseX    = CursorPos.X;
}

// ---------------------------------------------------------------------------

void AFP_CharacterPreviewActor::LoadFromCharacterRecord(const FFP_CharacterSaveRecord& CharacterRecord)
{
	SetActorHiddenInGame(false);
	ResetToDefault();

	if (!CharacterRecord.CharacterID.IsValid()) return;

	const FString SlotName = FString::Printf(TEXT("FP_Inventory_%s"),
		*CharacterRecord.CharacterID.ToString(EGuidFormats::DigitsWithHyphens));

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0)) return;

	UFP_InventorySaveData* SaveData = Cast<UFP_InventorySaveData>(
		UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	if (!SaveData) return;

	for (const FFP_EquippedItemSaveRecord& Record : SaveData->EquippedItems)
	{
		const FFP_ItemManifest* Manifest = Record.ItemManifest.GetPtr<FFP_ItemManifest>();
		if (!Manifest) continue;

		// Apply mesh entries
		if (const FFP_MeshFragment* MeshFrag = Manifest->GetFragmentOfType<FFP_MeshFragment>())
		{
			for (const FFP_MeshEntry& Entry : MeshFrag->GetMeshEntries())
			{
				if (Entry.bReplaceMesh)
				{
					if (USkeletalMeshComponent* BodyMesh = GetBodyPartMesh(Entry.BodyPart))
						BodyMesh->SetSkeletalMeshAsset(Entry.Mesh);
				}
				else
				{
					if (!IsValid(Entry.Mesh) || Entry.Socket == NAME_None) continue;

					USkeletalMeshComponent* AttachComp = NewObject<USkeletalMeshComponent>(this);
					AttachComp->SetSkeletalMeshAsset(Entry.Mesh);
					AttachComp->RegisterComponent();
					AttachComp->AttachToComponent(
						Mesh_Body,
						FAttachmentTransformRules::SnapToTargetNotIncludingScale,
						Entry.Socket);

					if (!Entry.WeaponAlignSocket.IsNone())
					{
						FTransform GripLocal = AttachComp->GetSocketTransform(Entry.WeaponAlignSocket, RTS_Component);
						AttachComp->SetRelativeTransform(GripLocal.Inverse());
					}

					if (!Entry.LeftHandSocket.IsNone())
					{
						LeftHandIKWeaponMesh        = AttachComp;
						LeftHandIKSocket            = Entry.LeftHandSocket;
						LeftHandJointTargetLocation = Entry.LeftHandJointTarget;
					}

					SpawnedAttachments.Add(AttachComp);
				}
			}
		}

		// Read grip stance from weapon slot
		if (const FFP_GripStanceFragment* GripFrag = Manifest->GetFragmentOfType<FFP_GripStanceFragment>())
			PreviewGripStance = GripFrag->GetGripStance();
	}
}

void AFP_CharacterPreviewActor::ResetToDefault()
{
	ClearAttachments();
	PreviewGripStance = EWeaponGripStyle::Unarmed;

	auto ApplyDefault = [](USkeletalMeshComponent* Comp, USkeletalMesh* Default)
	{
		if (IsValid(Comp) && IsValid(Default)) Comp->SetSkeletalMeshAsset(Default);
	};

	ApplyDefault(Mesh_Head,  Default_Head);
	ApplyDefault(Mesh_Torso, Default_Torso);
	ApplyDefault(Mesh_Arms,  Default_Arms);
	ApplyDefault(Mesh_Hands, Default_Hands);
	ApplyDefault(Mesh_Legs,  Default_Legs);
	ApplyDefault(Mesh_Feet,  Default_Feet);
}

void AFP_CharacterPreviewActor::SetPreviewGripStance(EWeaponGripStyle InGripStance)
{
	PreviewGripStance = InGripStance;
}

// ---------------------------------------------------------------------------

void AFP_CharacterPreviewActor::ClearAttachments()
{
	for (TObjectPtr<USkeletalMeshComponent>& Comp : SpawnedAttachments)
	{
		if (IsValid(Comp))
		{
			Comp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Comp->DestroyComponent();
		}
	}
	SpawnedAttachments.Empty();

	LeftHandIKWeaponMesh        = nullptr;
	LeftHandIKSocket            = NAME_None;
	LeftHandJointTargetLocation = FVector::ZeroVector;
}

USkeletalMeshComponent* AFP_CharacterPreviewActor::GetBodyPartMesh(EBodyPart BodyPart) const
{
	switch (BodyPart)
	{
	case EBodyPart::Head:  return Mesh_Head;
	case EBodyPart::Torso: return Mesh_Torso;
	case EBodyPart::Arms:  return Mesh_Arms;
	case EBodyPart::Hands: return Mesh_Hands;
	case EBodyPart::Legs:  return Mesh_Legs;
	case EBodyPart::Feet:  return Mesh_Feet;
	default:               return nullptr;
	}
}

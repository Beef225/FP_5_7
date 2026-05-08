// Copyright JG

#include "Preview/FP_CharacterPreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Inventory/Items/Fragments/FP_GripStanceFragment.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "SaveSystem/FP_InventorySaveData.h"

AFP_CharacterPreviewActor::AFP_CharacterPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

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
}

// ---------------------------------------------------------------------------

void AFP_CharacterPreviewActor::LoadFromInventorySave(UFP_InventorySaveData* SaveData)
{
	if (!SaveData) return;

	ResetToDefault();

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

	Mesh_Head ->SetSkeletalMeshAsset(Default_Head);
	Mesh_Torso->SetSkeletalMeshAsset(Default_Torso);
	Mesh_Arms ->SetSkeletalMeshAsset(Default_Arms);
	Mesh_Hands->SetSkeletalMeshAsset(Default_Hands);
	Mesh_Legs ->SetSkeletalMeshAsset(Default_Legs);
	Mesh_Feet ->SetSkeletalMeshAsset(Default_Feet);
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

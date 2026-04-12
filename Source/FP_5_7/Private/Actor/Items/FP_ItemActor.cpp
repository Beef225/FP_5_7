// Copyright JG

#include "Actor/Items/FP_ItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Inventory/Items/FP_ItemComponent.h"
#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "UI/Widget/Items/FP_ItemPickupWidget.h"

AFP_ItemActor::AFP_ItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	ItemComponent = CreateDefaultSubobject<UFP_ItemComponent>(TEXT("ItemComponent"));

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(Mesh);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetDrawAtDesiredSize(true);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(Mesh);
	NiagaraComponent->bAutoActivate = false;
}

void AFP_ItemActor::BeginPlay()
{
	Super::BeginPlay();

	// Notify all fragments that the item has spawned. For editor-placed items the manifest
	// is already populated from Blueprint defaults; for code-spawned items InitItemManifest
	// was called before FinishSpawning (deferred spawn), so both cases are safe here.
	ItemComponent->GetItemManifestMutable().NotifyItemSpawned();

	InitialisePickupWidget();

	if (SpawnEffect)
	{
		NiagaraComponent->SetAsset(SpawnEffect);
		NiagaraComponent->Activate();
	}
}

void AFP_ItemActor::InitialisePickupWidget()
{
	if (!PickupWidgetClass) return;

	WidgetComponent->SetWidgetClass(PickupWidgetClass);

	if (UFP_ItemPickupWidget* PickupWidget = Cast<UFP_ItemPickupWidget>(WidgetComponent->GetUserWidgetObject()))
	{
		PickupWidget->SetItemActor(this);
	}
}

void AFP_ItemActor::OnPickupRequested()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC)) return;

	UFP_InventoryComponent* IC = UFP_AbilitySystemLibrary::GetInventoryComponent(PC);
	if (!IsValid(IC)) return;

	IC->TryAddItem(ItemComponent);
	Destroy();
}

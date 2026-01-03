// Copyright JG


#include "Actor/FP_EffectActor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "Components/SphereComponent.h"


// Sets default values
AFP_EffectActor::AFP_EffectActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
}

void AFP_EffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//TODO: Change this to apply a Gameplay Effect. For now, using const_cast as a hack!
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		const UFP_AttributeSet* FP_AttributeSet = Cast<UFP_AttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UFP_AttributeSet::StaticClass()));

		UFP_AttributeSet* MutableFP_AttributeSet = const_cast<UFP_AttributeSet*>(FP_AttributeSet);
		MutableFP_AttributeSet->SetHitPoints(FP_AttributeSet->GetHitPoints() + 25.f);
		Destroy();


	}
}

void AFP_EffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

// Called when the game starts or when spawned
void AFP_EffectActor::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AFP_EffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AFP_EffectActor::EndOverlap);
	
}

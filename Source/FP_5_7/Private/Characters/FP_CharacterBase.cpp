// Copyright JG
#include "Characters/FP_CharacterBase.h"

// Sets default values
AFP_CharacterBase::AFP_CharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AFP_CharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AFP_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFP_CharacterBase::InitAbilityActorInfo()
{
}




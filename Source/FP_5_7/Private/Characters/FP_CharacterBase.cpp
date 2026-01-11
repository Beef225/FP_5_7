// Copyright JG
#include "Characters/FP_CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"

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

void AFP_CharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
	
}

void AFP_CharacterBase::InitializeDefaultAttributes() const
{
	
	ApplyEffectToSelf(DefaultPrimaryAttributes,1.0f);
	ApplyEffectToSelf(DefaultSecondaryAttributes,1.0f);
	ApplyEffectToSelf(PrimaryAttributeDerivedBonuses,1.0f);
	ApplyEffectToSelf(DefaultVitalAttributes,1.0f);
	
	
}

void AFP_CharacterBase::AddCharacterAbilities()
{
	UFP_AbilitySystemComponent* FP_ASC = CastChecked<UFP_AbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	FP_ASC->AddCharacterAbilities(StartupAbilities);
	
	
}






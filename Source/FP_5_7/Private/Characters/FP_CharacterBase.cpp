// Copyright JG

#include "Characters/FP_CharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "FP_5_7/FP_5_7.h"
#include "AbilitySystem/FP_AttributeSet.h"

// If you want to use your tag singleton here, uncomment and set SkillMoveSpeedTag in ctor or BP defaults.
// #include "FP_GameplayTags.h"

// Sets default values
AFP_CharacterBase::AFP_CharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);


	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Option A: set in BP defaults per character class (recommended while iterating)
	// SkillMoveSpeedTag = ...;

	// Option B: set from your singleton (if you have the tag defined)
	// SkillMoveSpeedTag = FFP_GameplayTags::Get().State_SkillAllowsMovement;
}

UAbilitySystemComponent* AFP_CharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AFP_CharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AFP_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Capture baseline walk speed (in case you want to bind later).
	if (!bCachedBaseWalkSpeed && GetCharacterMovement())
	{
		CachedBaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		bCachedBaseWalkSpeed = true;
	}
}

void AFP_CharacterBase::InitAbilityActorInfo()
{
	// Base class does not know how your ASC/AttributeSet are created/owned.
	// Child classes should set AbilitySystemComponent + AttributeSet and initialize ASC actor info,
	// then call this function (or call the functions below explicitly).

	// If the child hasn't assigned these yet, do nothing.
	if (!AbilitySystemComponent || !AttributeSet)
	{
		return;
	}

	// Capture baseline if not already.
	if (!bCachedBaseWalkSpeed && GetCharacterMovement())
	{
		CachedBaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		bCachedBaseWalkSpeed = true;
	}

	// Bind delegates + apply initial speed.
	BindMovementSpeedCallbacks();
	RefreshMovementSpeed();
}

FVector AFP_CharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AFP_CharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle =
		GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);

	// Apply to self
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AFP_CharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.0f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.0f);
	ApplyEffectToSelf(PrimaryAttributeDerivedBonuses, 1.0f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.0f);
}

void AFP_CharacterBase::AddCharacterAbilities()
{
	UFP_AbilitySystemComponent* FP_ASC = CastChecked<UFP_AbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	FP_ASC->AddCharacterAbilities(StartupAbilities);
}

void AFP_CharacterBase::BindMovementSpeedCallbacks()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// If your AttributeSet isn't your FP set, we can't bind to FP attributes.
	const UFP_AttributeSet* FPAS = Cast<UFP_AttributeSet>(AttributeSet);
	if (!FPAS)
	{
		return;
	}

	// Attribute change delegates (signature: void(const FOnAttributeChangeData&))
	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UFP_AttributeSet::GetMovementSpeedAttribute())
		.AddUObject(this, &AFP_CharacterBase::OnMoveSpeedAttributeChanged);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UFP_AttributeSet::GetSkillMoveSpeedModifierAttribute())
		.AddUObject(this, &AFP_CharacterBase::OnMoveSpeedAttributeChanged);

	// Tag add/remove delegate (signature: void(FGameplayTag, int32))
	if (SkillMoveSpeedTag.IsValid())
	{
		AbilitySystemComponent
			->RegisterGameplayTagEvent(SkillMoveSpeedTag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &AFP_CharacterBase::OnSkillMoveSpeedTagChanged);
	}
}

void AFP_CharacterBase::OnMoveSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	RefreshMovementSpeed();
}

void AFP_CharacterBase::OnSkillMoveSpeedTagChanged(FGameplayTag Tag, int32 NewCount)
{
	RefreshMovementSpeed();
}

void AFP_CharacterBase::RefreshMovementSpeed()
{
	if (!GetCharacterMovement())
	{
		return;
	}

	const UFP_AttributeSet* FPAS = Cast<UFP_AttributeSet>(AttributeSet);
	if (!FPAS)
	{
		return;
	}

	// Ensure we have a baseline.
	if (!bCachedBaseWalkSpeed)
	{
		CachedBaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		bCachedBaseWalkSpeed = true;
	}

	// MovementSpeed is a *delta percent* stored as additive (e.g. 0.3 = +30%, -0.2 = -20%)
	const float MoveSpeedDelta = FPAS->GetMovementSpeed();

	// Base * (1 + delta)
	float FinalSpeed = CachedBaseWalkSpeed * (1.0f + MoveSpeedDelta);

	// Prevent weird negatives (you can clamp tighter if desired).
	FinalSpeed = FMath::Max(0.0f, FinalSpeed);

	// Optional skill movement modifier:
	// - SkillSpeedMovementModifier is intended as a multiplier (e.g. 0.5 = 50% speed while performing skill movement)
	// - Apply only when a gameplay tag indicates "skill allows movement" (you’ll decide the tag + when to apply it).
	if (AbilitySystemComponent && SkillMoveSpeedTag.IsValid())
	{
		const bool bInSkillMoveMode = AbilitySystemComponent->HasMatchingGameplayTag(SkillMoveSpeedTag);
		if (bInSkillMoveMode)
		{
			// Example: FinalSpeed(900) * 0.5 = 450
			const float SkillMoveMultiplier = FPAS->GetSkillMoveSpeedModifier();
			FinalSpeed *= SkillMoveMultiplier;
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = FinalSpeed;
}

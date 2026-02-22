// Copyright JG

#include "Characters/FP_CharacterBase.h"

#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "FP_5_7/FP_5_7.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "Kismet/GameplayStatics.h"


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

}

UAbilitySystemComponent* AFP_CharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AFP_CharacterBase::GetDeathMontage_Implementation()
{
	return DeathMontage;
}

UAnimMontage* AFP_CharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

USoundBase* AFP_CharacterBase::GetHitReactSound_Implementation()
{
	return HitReactSound;
}

USoundBase* AFP_CharacterBase::GetDeathSound_Implementation()
{
	return DeathSound;
}

void AFP_CharacterBase::MulticastPlayDeathSound_Implementation()
{
	if (!DeathSound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());

}

void AFP_CharacterBase::Die()
{
	if (bDead)
	{
		return;
	}
	bDead = true;

	// Detach weapon immediately so montage doesn't drag it around (optional)
	if (Weapon)
	{
		Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	}
	
	MulticastPlayDeathSound();

	// If no montage (or no anim instance), ragdoll immediately
	const float Delay = GetDeathRagdollDelay();
	if (Delay <= 0.f)
	{
		MulticastHandleDeath();
		return;
	}

	// Play montage everywhere
	MulticastPlayDeathMontage(DeathMontage);

	// Server schedules the ragdoll moment
	if (HasAuthority())
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(
			TimerHandle,
			this,
			&AFP_CharacterBase::MulticastHandleDeath,
			Delay,
			false
		);
	}
}

float AFP_CharacterBase::GetDeathRagdollDelay() const
{
	if (!DeathMontage) return 0.f;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return 0.f;

	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst) return 0.f;

	const float Rate = FMath::Max(0.01f, DeathMontagePlayRate);
	return DeathMontage->GetPlayLength() / Rate;
}

void AFP_CharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
		StartDissolveTimeline(DynamicMatInst);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);
		StartWeaponDissolveTimeline(DynamicMatInst);
	}

}

void AFP_CharacterBase::MulticastPlayDeathMontage_Implementation(UAnimMontage* Montage)
{
	if (!Montage) return;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// If already ragdolled, montage won't play
	if (MeshComp->IsSimulatingPhysics())
	{
		return;
	}

	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst) return;

	AnimInst->Montage_Play(Montage, DeathMontagePlayRate);
}

void AFP_CharacterBase::MulticastHandleDeath_Implementation()
{
	if (Weapon!=nullptr)
	{
		Weapon->SetSimulatePhysics(true);
		Weapon->SetEnableGravity(true);
		Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Weapon->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	}
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
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

FVector AFP_CharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FFP_GameplayTags& GameplayTags = FFP_GameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.Montage_Skill_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.Montage_Skill_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.Montage_Skill_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.Montage_Skill_Tail))
	{
		return GetMesh()->GetSocketLocation(TailTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.Montage_Skill_Mouth))
	{
		return GetMesh()->GetSocketLocation(MouthSocketName);
	}
	return FVector();
}

bool AFP_CharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AFP_CharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AFP_CharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
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

	const FFP_GameplayTags& GameplayTags = FFP_GameplayTags::Get();

	// Tag add/remove delegate (signature: void(FGameplayTag, int32))
	AbilitySystemComponent
		->RegisterGameplayTagEvent(GameplayTags.Skill_MoveSpeed_Diminished, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AFP_CharacterBase::OnSkillMoveSpeedTagChanged);

	AbilitySystemComponent
		->RegisterGameplayTagEvent(GameplayTags.Skill_MoveSpeed_None, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AFP_CharacterBase::OnSkillMoveSpeedTagChanged);

}

void AFP_CharacterBase::OnMoveSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	(void)Data;
	RefreshMovementSpeed();
}

void AFP_CharacterBase::OnSkillMoveSpeedTagChanged(FGameplayTag Tag, int32 NewCount)
{
	(void)Tag;
	(void)NewCount;
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

	if (AbilitySystemComponent)
	{
		const FFP_GameplayTags& GameplayTags = FFP_GameplayTags::Get();

		if (AbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.Skill_MoveSpeed_None))
		{
			FinalSpeed = 0.0f;
		}
		else if (AbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.Skill_MoveSpeed_Diminished))

		{
			const float SkillMoveMultiplier = FPAS->GetSkillMoveSpeedModifier();
			FinalSpeed *= SkillMoveMultiplier;
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = FinalSpeed;
}

// Copyright JG

#include "AbilitySystem/Abilities/Templates/FP_ProjectileSkill.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actor/Skills/FP_Projectile.h"
#include "Interaction/FP_CombatInterface.h"
#include "FP_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"

void UFP_ProjectileSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

FVector UFP_ProjectileSkill::GetProjectileSpawnLocation() const
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return FVector::ZeroVector;
	if (!Avatar->Implements<UFP_CombatInterface>()) return Avatar->GetActorLocation();
	return IFP_CombatInterface::Execute_GetCombatSocketLocation(Avatar, FFP_GameplayTags::Get().Montage_Skill_Weapon);
}

void UFP_ProjectileSkill::PlayFireFX() const
{
	UFP_WeaponFXLibrary::PlayWeaponFireFX(GetAvatarActorFromActorInfo(), FireFX);
}

void UFP_ProjectileSkill::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !Avatar->HasAuthority()) return;
	if (!ProjectileClass) return;
	if (!Avatar->Implements<UFP_CombatInterface>()) return;

	const FVector SocketLocation = GetProjectileSpawnLocation();

	// Fires along the avatar's facing (the character already turns to the cursor).
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Avatar->GetActorQuat());

	AFP_Projectile* Projectile = GetWorld()->SpawnActorDeferred<AFP_Projectile>(
		ProjectileClass,
		SpawnTransform,
		/*Owner*/ GetOwningActorFromActorInfo(),
		/*Instigator*/ Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Projectile) return;

	InitProjectile(Projectile, ProjectileTargetLocation);
	Projectile->FinishSpawning(SpawnTransform);
}

AFP_Projectile* UFP_ProjectileSkill::SpawnProjectileInDirection(const FVector& Origin, const FVector& Direction,
	int32 ShotIndex, int32 ShotGroupId, int32 Generation)
{
	return SpawnProjectileIgnoring(Origin, Direction, ShotIndex, ShotGroupId, Generation, TArray<AActor*>());
}

AFP_Projectile* UFP_ProjectileSkill::SpawnProjectileIgnoring(const FVector& Origin, const FVector& Direction,
	int32 ShotIndex, int32 ShotGroupId, int32 Generation, const TArray<AActor*>& IgnoreActors)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !Avatar->HasAuthority() || !ProjectileClass) return nullptr;

	FVector Dir(Direction.X, Direction.Y, 0.f);
	if (!Dir.Normalize()) Dir = Avatar->GetActorForwardVector();

	const FTransform SpawnTransform(Dir.Rotation(), Origin);

	AFP_Projectile* Projectile = GetWorld()->SpawnActorDeferred<AFP_Projectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Projectile) return nullptr;

	InitProjectile(Projectile, Origin + Dir * 1000.f);
	Projectile->ShotIndex   = ShotIndex;
	Projectile->ShotGroupId = ShotGroupId;
	Projectile->Generation  = Generation;
	for (AActor* Ignore : IgnoreActors)
	{
		Projectile->AddIgnoredTarget(Ignore);
	}

	Projectile->FinishSpawning(SpawnTransform);
	return Projectile;
}

void UFP_ProjectileSkill::InitProjectile(AFP_Projectile* Projectile, const FVector& TargetLocation)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();

	// ProjectileSpeed is a delta (+0.3 = +30%), gated for this skill; skill tree adds on top.
	bool bFound = false;
	const float SpeedDelta = GetSourceAttributeValue(UFP_AttributeSet::GetProjectileSpeedAttribute(), bFound);
	if (!bFound) { UE_LOG(LogTemp, Warning, TEXT("ProjectileSkill: Could not read ProjectileSpeed attribute.")); }
	Projectile->InitSpeedFromDelta((bFound ? SpeedDelta : 0.f) + GetExtraProjectileSpeedDelta());
	Projectile->SetSourceActor(Avatar);

	Projectile->PierceRemaining = GetEffectivePierceCount();
	Projectile->ChainRemaining  = GetEffectiveChainCount();
	Projectile->ChainRadius     = GetEffectiveChainRadius();
	Projectile->OwningSkill     = this;

	// Fallback spec, used only if this ability is gone when the projectile lands.
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Avatar);
	if (!SourceASC || !DamageEffectClass) return;

	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);
	FHitResult HitResult;
	HitResult.Location = TargetLocation;
	EffectContextHandle.AddHitResult(HitResult);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	AssignRolledDamageMagnitudes(SpecHandle);
	Projectile->DamageEffectSpecHandle = SpecHandle;
}

void UFP_ProjectileSkill::HandleProjectileHit(AFP_Projectile* Projectile, AActor* Target, const FHitResult& Hit,
                                              const FFP_ProjectileHitInfo& Info)
{
	if (!Target || !DamageEffectClass) return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!SourceASC || !TargetASC) return;

	// Fresh roll per hit, so pierced / chained hits each roll their own damage and crit.
	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	Context.SetAbility(this);
	if (Projectile)
	{
		Context.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		Context.AddActors(Actors);
	}
	Context.AddHitResult(Hit);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), Context);
	if (!SpecHandle.IsValid()) return;

	const float Multiplier = FMath::Max(GetProjectileHitDamageMultiplier(Projectile, Target, Info), 0.f);
	AssignRolledDamageMagnitudes(SpecHandle, Multiplier);
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

	OnProjectileHitApplied(Projectile, Target, Hit, Info);
}

int32 UFP_ProjectileSkill::GetEffectivePierceCount() const
{
	bool bFound = false;
	const float Stat = GetSourceAttributeValue(UFP_AttributeSet::GetProjectileTargetsPiercedAttribute(), bFound);
	return FMath::Max((bFound ? FMath::RoundToInt(Stat) : 0) + GetExtraPierceCount(), 0);
}

int32 UFP_ProjectileSkill::GetEffectiveChainCount() const
{
	bool bFound = false;
	const float Stat = GetSourceAttributeValue(UFP_AttributeSet::GetProjectileTargetsBouncedAttribute(), bFound);
	return FMath::Max((bFound ? FMath::RoundToInt(Stat) : 0) + GetExtraChainCount(), 0);
}

int32 UFP_ProjectileSkill::GetAdditionalProjectileCount() const
{
	bool bFound = false;
	const float Stat = GetSourceAttributeValue(UFP_AttributeSet::GetProjectileCountAttribute(), bFound);
	return bFound ? FMath::Max(FMath::RoundToInt(Stat), 0) : 0;
}

// Copyright JG

#include "AbilitySystem/Abilities/Templates/FP_ProjectileSkill.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actor/Skills/FP_Projectile.h"
#include "Interaction/FP_CombatInterface.h"
#include "FP_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "GameFramework/Pawn.h"

void UFP_ProjectileSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
}

void UFP_ProjectileSkill::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	const bool bIsServer = Avatar->HasAuthority();
	if (!bIsServer) return;

	if (!ProjectileClass) return;

	IFP_CombatInterface* CombatInterface = Cast<IFP_CombatInterface>(Avatar);
	if (!CombatInterface) return;

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	Rotation.Pitch = 0.f;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);

	SpawnTransform.SetRotation(Rotation.Quaternion());
	SpawnTransform.SetRotation(Avatar->GetActorQuat());

	AFP_Projectile* Projectile = GetWorld()->SpawnActorDeferred<AFP_Projectile>(
		ProjectileClass,
		SpawnTransform,
		/*Owner*/ GetOwningActorFromActorInfo(),
		/*Instigator*/ Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Projectile)
	{
		return;
	}

	// --- Read ProjectileSpeed from the ability owner's ASC as a DELTA (e.g., +0.3 for +30%) ---
	float SpeedDelta = 0.f;
	{
		bool bFound = false;
		SpeedDelta = GetSourceAttributeValue(UFP_AttributeSet::GetProjectileSpeedAttribute(), bFound);

		// Optional: if you want to detect missing ASC/invalid attr, bFound will be false.
		if (!bFound) { UE_LOG(LogTemp, Warning, TEXT("ProjectileSkill: Could not read ProjectileSpeed attribute.")); }
	}

	// Apply to projectile before FinishSpawning (your projectile should implement this)
	Projectile->InitSpeedFromDelta(SpeedDelta);
	Projectile->SetSourceActor(Avatar);

	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);
	FHitResult HitResult;
	HitResult.Location = ProjectileTargetLocation;
	EffectContextHandle.AddHitResult(HitResult);
	
	
		
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	
	const FFP_GameplayTags GameplayTags = FFP_GameplayTags::Get();
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	
	for (auto& Pair: DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key,ScaledDamage);
	}
		
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaledDamage);

	
	Projectile->DamageEffectSpecHandle = SpecHandle;

	Projectile->FinishSpawning(SpawnTransform);
}

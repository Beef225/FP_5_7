// Copyright JG


#include "AbilitySystem/Abilities/Templates/FP_ProjectileSkill.h"
#include "Actor/Skills/FP_Projectile.h"
#include "Interaction/FP_CombatInterface.h"

void UFP_ProjectileSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (!bIsServer) return;

	IFP_CombatInterface* CombatInterface = Cast<IFP_CombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		//TODO: Set the Projectile Rotation
		
		AFP_Projectile* Projectile = GetWorld()->SpawnActorDeferred<AFP_Projectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		//TODO: Give the Projectile a Gameplay Effect Spec for causing Damage.
		
		Projectile->FinishSpawning(SpawnTransform);
	}

}

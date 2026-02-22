// Copyright JG


#include "Player/FP_PlayerState.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "Net/UnrealNetwork.h"

AFP_PlayerState::AFP_PlayerState()
{
	
	AbilitySystemComponent = CreateDefaultSubobject<UFP_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UFP_AttributeSet>("AttributeSet");
	
	SetNetUpdateFrequency(100.f);
}

void AFP_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFP_PlayerState, Level);
	DOREPLIFETIME(AFP_PlayerState, GripStance);
}

UAbilitySystemComponent* AFP_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AFP_PlayerState::SetGripStance(EWeaponGripStyle InGripStance)
{
	if (GripStance == InGripStance)
	{
		return;
	}

	GripStance = InGripStance;
	ForceNetUpdate();

}

void AFP_PlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AFP_PlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AFP_PlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AFP_PlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AFP_PlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level);
}

void AFP_PlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}

void AFP_PlayerState::OnRep_GripStance(EWeaponGripStyle OldGripStance)
{
	
}



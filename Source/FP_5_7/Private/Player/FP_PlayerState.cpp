// Copyright JG


#include "Player/FP_PlayerState.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/Data/FP_LevelUpInfo.h"
#include "FP_GameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"

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
	DOREPLIFETIME(AFP_PlayerState, XP);
	DOREPLIFETIME(AFP_PlayerState, AttributePoints);
	DOREPLIFETIME(AFP_PlayerState, MightPassivePoints);
	DOREPLIFETIME(AFP_PlayerState, ResonancePassivePoints);
	DOREPLIFETIME(AFP_PlayerState, AgilityPassivePoints);
	DOREPLIFETIME(AFP_PlayerState, FortitudePassivePoints);
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
	if (LevelUpInfo)
	{
		const int32 MaxLevel = LevelUpInfo->GetMaxLevel();
		const int32 MaxXP = LevelUpInfo->LevelUpInformation[MaxLevel].LevelUpRequirement;
		XP = FMath::Min(XP + InXP, MaxXP);
	}
	else
	{
		XP += InXP;
	}
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

void AFP_PlayerState::AddToAttributePoints(int32 InPoints)
{
	AttributePoints += InPoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AFP_PlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AFP_PlayerState::AddToPassivePoints(const FGameplayTag& AttributeTag, int32 InPoints)
{
	const FFP_GameplayTags& GameplayTags = FFP_GameplayTags::Get();
	if (AttributeTag.MatchesTagExact(GameplayTags.Attributes_Primary_Might))
	{
		MightPassivePoints += InPoints;
		OnPassivePointsChangedDelegate.Broadcast(GameplayTags.Attributes_Primary_Might, MightPassivePoints);
	}
	else if (AttributeTag.MatchesTagExact(GameplayTags.Attributes_Primary_Resonance))
	{
		ResonancePassivePoints += InPoints;
		OnPassivePointsChangedDelegate.Broadcast(GameplayTags.Attributes_Primary_Resonance, ResonancePassivePoints);
	}
	else if (AttributeTag.MatchesTagExact(GameplayTags.Attributes_Primary_Agility))
	{
		AgilityPassivePoints += InPoints;
		OnPassivePointsChangedDelegate.Broadcast(GameplayTags.Attributes_Primary_Agility, AgilityPassivePoints);
	}
	else if (AttributeTag.MatchesTagExact(GameplayTags.Attributes_Primary_Fortitude))
	{
		FortitudePassivePoints += InPoints;
		OnPassivePointsChangedDelegate.Broadcast(GameplayTags.Attributes_Primary_Fortitude, FortitudePassivePoints);
	}
}

void AFP_PlayerState::OnRep_MightPassivePoints(int32 OldPoints)
{
	OnPassivePointsChangedDelegate.Broadcast(FFP_GameplayTags::Get().Attributes_Primary_Might, MightPassivePoints);
}

void AFP_PlayerState::OnRep_ResonancePassivePoints(int32 OldPoints)
{
	OnPassivePointsChangedDelegate.Broadcast(FFP_GameplayTags::Get().Attributes_Primary_Resonance, ResonancePassivePoints);
}

void AFP_PlayerState::OnRep_AgilityPassivePoints(int32 OldPoints)
{
	OnPassivePointsChangedDelegate.Broadcast(FFP_GameplayTags::Get().Attributes_Primary_Agility, AgilityPassivePoints);
}

void AFP_PlayerState::OnRep_FortitudePassivePoints(int32 OldPoints)
{
	OnPassivePointsChangedDelegate.Broadcast(FFP_GameplayTags::Get().Attributes_Primary_Fortitude, FortitudePassivePoints);
}

void AFP_PlayerState::OnRep_GripStance(EWeaponGripStyle OldGripStance)
{

}

void AFP_PlayerState::LoadAllocatedPoints(int32 InUnspent, int32 InMight, int32 InResonance, int32 InAgility, int32 InFortitude)
{
	// Restore unspent pool
	AttributePoints = InUnspent;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);

	// Restore per-attribute counters and broadcast so UI refreshes
	MightPassivePoints     = InMight;
	ResonancePassivePoints = InResonance;
	AgilityPassivePoints   = InAgility;
	FortitudePassivePoints = InFortitude;

	const FFP_GameplayTags& FPTags = FFP_GameplayTags::Get();
	OnPassivePointsChangedDelegate.Broadcast(FPTags.Attributes_Primary_Might,     MightPassivePoints);
	OnPassivePointsChangedDelegate.Broadcast(FPTags.Attributes_Primary_Resonance, ResonancePassivePoints);
	OnPassivePointsChangedDelegate.Broadcast(FPTags.Attributes_Primary_Agility,   AgilityPassivePoints);
	OnPassivePointsChangedDelegate.Broadcast(FPTags.Attributes_Primary_Fortitude, FortitudePassivePoints);

	// Apply the saved points as base value increases on top of the defaults
	// already set by InitializeDefaultAttributes.
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) return;

	auto AddBase = [ASC](FGameplayAttribute Attr, int32 Points)
	{
		if (Points > 0)
		{
			ASC->SetNumericAttributeBase(Attr, ASC->GetNumericAttributeBase(Attr) + static_cast<float>(Points));
		}
	};

	AddBase(UFP_AttributeSet::GetMightAttribute(),     InMight);
	AddBase(UFP_AttributeSet::GetResonanceAttribute(), InResonance);
	AddBase(UFP_AttributeSet::GetAgilityAttribute(),   InAgility);
	AddBase(UFP_AttributeSet::GetFortitudeAttribute(), InFortitude);
}



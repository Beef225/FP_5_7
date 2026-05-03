// Copyright JG


#include "Player/FP_PlayerState.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/Data/FP_LevelUpInfo.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "AbilitySystem/Data/FP_SkillLevelUpInfo.h"
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

int32 AFP_PlayerState::GetPassivePointsForTree(const FGameplayTag& TreeTag) const
{
	const FFP_GameplayTags& T = FFP_GameplayTags::Get();
	if (TreeTag == T.PassiveTree_Might)     return MightPassivePoints;
	if (TreeTag == T.PassiveTree_Resonance) return ResonancePassivePoints;
	if (TreeTag == T.PassiveTree_Agility)   return AgilityPassivePoints;
	if (TreeTag == T.PassiveTree_Fortitude) return FortitudePassivePoints;
	return 0;
}

void AFP_PlayerState::SpendPassivePoints(const FGameplayTag& TreeTag, int32 Amount)
{
	const FFP_GameplayTags& T = FFP_GameplayTags::Get();
	FGameplayTag AttrTag;
	if      (TreeTag == T.PassiveTree_Might)     AttrTag = T.Attributes_Primary_Might;
	else if (TreeTag == T.PassiveTree_Resonance) AttrTag = T.Attributes_Primary_Resonance;
	else if (TreeTag == T.PassiveTree_Agility)   AttrTag = T.Attributes_Primary_Agility;
	else if (TreeTag == T.PassiveTree_Fortitude) AttrTag = T.Attributes_Primary_Fortitude;

	if (AttrTag.IsValid())
		AddToPassivePoints(AttrTag, -Amount);
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

void AFP_PlayerState::InitGrantedSkillsFromLibrary()
{
	if (!SkillLibrary) return;
	for (const FFP_AbilityEntry& Entry : SkillLibrary->AbilityEntries)
	{
		if (Entry.bGranted && Entry.SkillTag.IsValid())
			GrantedSkillTags.Add(Entry.SkillTag);
	}
}

void AFP_PlayerState::AddGrantedSkill(const FGameplayTag& SkillTag)
{
	if (SkillTag.IsValid())
		GrantedSkillTags.Add(SkillTag);
}

void AFP_PlayerState::RemoveGrantedSkill(const FGameplayTag& SkillTag)
{
	GrantedSkillTags.Remove(SkillTag);
}

void AFP_PlayerState::LoadSkillState(
	const TMap<FGameplayTag, int32>& InXP,
	const TMap<FGameplayTag, int32>& InLevels,
	const TMap<FGameplayTag, int32>& InPoints,
	const TMap<FGameplayTag, FGameplayTag>& InInputTags)
{
	SkillXP           = InXP;
	SkillLevel        = InLevels;
	UnspentSkillPoints = InPoints;
	SkillInputTags    = InInputTags;

	for (const auto& Pair : SkillXP)
		OnSkillXPChangedDelegate.Broadcast(Pair.Key, Pair.Value);

	for (const auto& Pair : SkillLevel)
		OnSkillLevelChangedDelegate.Broadcast(Pair.Key, Pair.Value);

	for (const auto& Pair : UnspentSkillPoints)
		OnSkillPointsChangedDelegate.Broadcast(Pair.Key, Pair.Value);
}

// Shared penalty core: given a pre-computed gap and player level, returns [0.01, 1.0].
static float XPPenaltyFromGap(int32 Gap, int32 PlayerLevel)
{
	const int32 Tolerance = 3 + PlayerLevel / 16;
	const float Excess = FMath::Max(static_cast<float>(Gap - Tolerance), 0.f);
	if (Excess <= 0.f) return 1.f;

	const float P = static_cast<float>(PlayerLevel);
	const float Base = P + 5.f;
	return FMath::Max(FMath::Pow(Base / (Base + FMath::Pow(Excess, 2.5f)), 1.5f), 0.01f);
}

float AFP_PlayerState::ComputeSkillXPMultiplier(int32 PlayerLevel, int32 MonsterLevel)
{
	if (MonsterLevel < 0) return 1.f;
	// One-directional: only penalise rushing (character below monster).
	// Overlevelled character farming low content => no penalty, enabling new-skill catch-up.
	return XPPenaltyFromGap(FMath::Max(MonsterLevel - PlayerLevel, 0), PlayerLevel);
}

float AFP_PlayerState::ComputeCharacterXPMultiplier(int32 PlayerLevel, int32 MonsterLevel)
{
	if (MonsterLevel < 0) return 1.f;
	// Bidirectional: penalise both farming low-level content AND rushing high-level content.
	return XPPenaltyFromGap(FMath::Abs(PlayerLevel - MonsterLevel), PlayerLevel);
}

void AFP_PlayerState::AddSkillXP(int32 InXP, int32 MonsterLevel)
{
	if (!SkillLibrary || InXP <= 0) return;

	const float Multiplier = ComputeSkillXPMultiplier(Level, MonsterLevel);
	const int32 AdjustedXP = (Multiplier >= 1.f)
		? InXP
		: FMath::Max(FMath::RoundToInt(static_cast<float>(InXP) * Multiplier), 1);

	for (const FFP_AbilityEntry& Entry : SkillLibrary->AbilityEntries)
	{
		if (!IsSkillGranted(Entry.SkillTag)) continue;
		if (!Entry.SkillLevelUpInfo) continue;
		if (!Entry.SkillTag.IsValid()) continue;

		const FGameplayTag& Tag = Entry.SkillTag;
		const UFP_SkillLevelUpInfo* Curve = Entry.SkillLevelUpInfo;
		const int32 HardCap = Entry.MaxLevel;

		int32 CurrentLevel = SkillLevel.FindOrAdd(Tag, 1);
		if (CurrentLevel >= HardCap) continue;

		const int32 MaxCurveLevel = FMath::Min(HardCap, Curve->GetMaxLevel());
		const int32 MaxXP = Curve->GetXPRequirementForLevel(MaxCurveLevel + 1);

		int32& CurrentXP = SkillXP.FindOrAdd(Tag, 0);
		CurrentXP = (MaxXP > 0) ? FMath::Min(CurrentXP + AdjustedXP, MaxXP) : CurrentXP + AdjustedXP;

		OnSkillXPChangedDelegate.Broadcast(Tag, CurrentXP);

		const int32 NewLevel = FMath::Min(Curve->FindLevelForXP(CurrentXP), HardCap);
		if (NewLevel > CurrentLevel)
		{
			int32& StoredLevel = SkillLevel.FindOrAdd(Tag, 1);
			for (int32 L = CurrentLevel + 1; L <= NewLevel; ++L)
			{
				const int32 Points = Curve->LevelUpInformation.IsValidIndex(L)
					? Curve->LevelUpInformation[L].SkillPointAward : 0;
				if (Points > 0)
				{
					UnspentSkillPoints.FindOrAdd(Tag, 0) += Points;
					OnSkillPointsChangedDelegate.Broadcast(Tag, UnspentSkillPoints[Tag]);
				}
			}
			StoredLevel = NewLevel;
			OnSkillLevelChangedDelegate.Broadcast(Tag, NewLevel);

			if (UFP_AbilitySystemComponent* FP_ASC = Cast<UFP_AbilitySystemComponent>(AbilitySystemComponent))
			{
				FScopedAbilityListLock Lock(*FP_ASC);
				for (FGameplayAbilitySpec& Spec : FP_ASC->GetActivatableAbilities())
				{
					if (FP_ASC->GetAbilityTagFromSpec(Spec).MatchesTagExact(Tag))
					{
						Spec.Level = NewLevel;
						FP_ASC->MarkAbilitySpecDirty(Spec);
						break;
					}
				}
			}
		}
	}
}


void AFP_PlayerState::AssignSkillToSlot(const FGameplayTag& SkillTag, const FGameplayTag& SlotInputTag)
{
	// Notify if a different skill was previously in this slot (so that slot's frame knows to clear)
	const FGameplayTag OldSkill = SkillInputTags.FindRef(SlotInputTag);
	if (OldSkill.IsValid() && !OldSkill.MatchesTagExact(SkillTag))
		OnSkillInputTagAssigned.Broadcast(SlotInputTag, FGameplayTag());

	SkillInputTags.Add(SlotInputTag, SkillTag);
	OnSkillInputTagAssigned.Broadcast(SlotInputTag, SkillTag);
}

void AFP_PlayerState::ClearSkillSlot(const FGameplayTag& SlotInputTag)
{
	if (!SkillInputTags.Contains(SlotInputTag)) return;

	SkillInputTags.Remove(SlotInputTag);
	OnSkillInputTagAssigned.Broadcast(SlotInputTag, FGameplayTag());
}

void AFP_PlayerState::ApplyInputTagsToASC(UFP_AbilitySystemComponent* ASC)
{
	if (!ASC || SkillInputTags.IsEmpty()) return;

	// Build reverse map: SkillTag → [SlotInputTags]
	TMap<FGameplayTag, TArray<FGameplayTag>> SkillToSlots;
	for (const auto& Pair : SkillInputTags)
	{
		if (Pair.Key.IsValid() && Pair.Value.IsValid())
			SkillToSlots.FindOrAdd(Pair.Value).Add(Pair.Key);
	}

	const FGameplayTag InputParent = FGameplayTag::RequestGameplayTag(FName("InputTag"));

	FScopedAbilityListLock Lock(*ASC);
	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		const FGameplayTag SkillTag = UFP_AbilitySystemComponent::GetAbilityTagFromSpec(Spec);
		if (!SkillTag.IsValid()) continue;

		const TArray<FGameplayTag>* Slots = SkillToSlots.Find(SkillTag);
		if (!Slots) continue; // not in saved map — leave existing tags untouched

		FGameplayTagContainer& DynTags = Spec.GetDynamicSpecSourceTags();
		bool bDirty = false;

		TArray<FGameplayTag> ToRemove;
		for (const FGameplayTag& Tag : DynTags)
			if (Tag.MatchesTag(InputParent))
				ToRemove.Add(Tag);
		for (const FGameplayTag& Tag : ToRemove) { DynTags.RemoveTag(Tag); bDirty = true; }

		for (const FGameplayTag& SlotTag : *Slots) { DynTags.AddTag(SlotTag); bDirty = true; }

		if (bDirty) ASC->MarkAbilitySpecDirty(Spec);
	}
}

void AFP_PlayerState::ApplyInputTagForSkill(UFP_AbilitySystemComponent* ASC, const FGameplayTag& SkillTag)
{
	if (!ASC || !SkillTag.IsValid()) return;

	for (const auto& Pair : SkillInputTags)
	{
		if (Pair.Value.MatchesTagExact(SkillTag))
			ASC->AddInputTagToSkill(SkillTag, Pair.Key);
	}
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



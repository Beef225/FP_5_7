// Copyright JG


#include "AbilitySystem/Abilities/FP_DamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "Characters/FP_CharacterBase.h"
#include "GameFramework/Pawn.h"
#include "Player/FP_PlayerState.h"

namespace
{
	bool ResolveSkillAbilityEntry(const UFP_DamageGameplayAbility* Ability, FFP_AbilityEntry& OutAbilityEntry)
	{
		if (Ability == nullptr)
		{
			return false;
		}

		const AActor* AvatarActor = Ability->GetAvatarActorFromActorInfo();
		const APawn* AvatarPawn = Cast<APawn>(AvatarActor);
		const AFP_PlayerState* FPPlayerState = AvatarPawn ? AvatarPawn->GetPlayerState<AFP_PlayerState>() : nullptr;
		if (FPPlayerState == nullptr)
		{
			return false;
		}

		const UFP_SkillLibrary* SkillLibrary = FPPlayerState->GetSkillLibrary();
		if (SkillLibrary == nullptr)
		{
			return false;
		}

		FGameplayTag SkillTag;
		const FGameplayTag SkillTagRoot = FGameplayTag::RequestGameplayTag(FName("Skill"));
		const FGameplayTag SkillsTagRoot = FGameplayTag::RequestGameplayTag(FName("Skills"));
		const FGameplayTagContainer& AssetTags = Ability->GetAssetTags();
		for (const FGameplayTag& Tag : AssetTags)
		{
			if (Tag.MatchesTag(SkillTagRoot) || Tag.MatchesTag(SkillsTagRoot))
			{
				SkillTag = Tag;
				break;
			}
		}

		if (!SkillTag.IsValid())
		{
			return false;
		}

		OutAbilityEntry = SkillLibrary->FindAbilityEntryForTag(SkillTag, false);
		return true;
	}
}


void UFP_DamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	AssignRolledDamageMagnitudes(DamageSpecHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	
}

float UFP_DamageGameplayAbility::GetSkillSpeedAttributeModifier() const
{
	FFP_AbilityEntry AbilityEntry;
	if (!ResolveSkillAbilityEntry(this, AbilityEntry))
	{
		return 1.f;
	}

	const float BaseSkillSpeed = FMath::Max(AbilityEntry.BaseSkillSpeed, 0.f);

	bool bFoundSkillSpeed = false;
	const float SkillSpeedModifier = GetSourceAttributeValue(UFP_AttributeSet::GetSkillSpeedAttribute(), bFoundSkillSpeed);
	const float AttributeMultiplier = bFoundSkillSpeed ? (1.f + SkillSpeedModifier) : 1.f;

	// Apply freeze ramp as a final independent multiplier so stacked skill speed
	// cannot resist the chill effect.
	float FreezeMultiplier = 1.f;
	if (const AFP_CharacterBase* CharBase = Cast<AFP_CharacterBase>(GetAvatarActorFromActorInfo()))
	{
		FreezeMultiplier = 1.f - CharBase->GetFreezeRamp();
	}

	return BaseSkillSpeed * AttributeMultiplier * FreezeMultiplier;
}


float UFP_DamageGameplayAbility::GetAoEAttributeModifier() const
{
	FFP_AbilityEntry AbilityEntry;
	if (!ResolveSkillAbilityEntry(this, AbilityEntry))
	{
		return 50.f;
	}

	const float BaseRadius = FMath::Max(AbilityEntry.BaseRadius, 0.f);

	bool bFoundAddRadius = false;
	const float AdditionalRadius = GetSourceAttributeValue(UFP_AttributeSet::GetAreaOfEffectAdditionalRadiusAttribute(), bFoundAddRadius);
	const float AdditionalRadiusScaled = bFoundAddRadius ? (AdditionalRadius * 100.f) : 0.f;

	bool bFoundAoEMultiplier = false;
	const float AoEMultiplier = GetSourceAttributeValue(UFP_AttributeSet::GetAreaOfEffectMulitplierAttribute(), bFoundAoEMultiplier);
	const float MultiplierTerm = FMath::Max((bFoundAoEMultiplier ? (1.f + AoEMultiplier) : 1.f), 0.0001f);

	return (BaseRadius + AdditionalRadiusScaled) * FMath::Sqrt(MultiplierTerm);
}

void UFP_DamageGameplayAbility::AppendSkillModifierTagsToDamageSpec(FGameplayEffectSpecHandle& DamageSpecHandle) const
{
	if (!DamageSpecHandle.Data.IsValid())
	{
		return;
	}

	FGameplayTag SkillTag;
	const FGameplayTag SkillTagRoot = FGameplayTag::RequestGameplayTag(FName("Skill"));
	const FGameplayTag SkillsTagRoot = FGameplayTag::RequestGameplayTag(FName("Skills"));
	for (const FGameplayTag& Tag : GetAssetTags())
	{
		if (Tag.MatchesTag(SkillTagRoot) || Tag.MatchesTag(SkillsTagRoot))
		{
			SkillTag = Tag;
			DamageSpecHandle.Data->AddDynamicAssetTag(Tag);
			break;
		}
	}

	if (!SkillTag.IsValid())
	{
		return;
	}

	FFP_AbilityEntry AbilityEntry;
	if (!ResolveSkillAbilityEntry(this, AbilityEntry))
	{
		return;
	}

	for (const FGameplayTag& ModifierTag : AbilityEntry.SkillModifierTags)
	{
		DamageSpecHandle.Data->AddDynamicAssetTag(ModifierTag);
	}
}



float UFP_DamageGameplayAbility::GetEffectiveSkillRate(float BaseRate) const
{
	bool bFound = false;
	const float SpeedDelta      = GetSourceAttributeValue(UFP_AttributeSet::GetSkillSpeedAttribute(), bFound);
	const float SpeedMultiplier = bFound ? (1.f + SpeedDelta) : 1.f;

	float FreezeMultiplier = 1.f;
	if (const AFP_CharacterBase* CharBase = Cast<AFP_CharacterBase>(GetAvatarActorFromActorInfo()))
		FreezeMultiplier = 1.f - CharBase->GetFreezeRamp();

	return BaseRate * SpeedMultiplier * FreezeMultiplier;
}

float UFP_DamageGameplayAbility::GetEffectiveSphereRadius(float BaseRadius) const
{
	bool bFound = false;
	const float Additional  = GetSourceAttributeValue(UFP_AttributeSet::GetAreaOfEffectAdditionalRadiusAttribute(), bFound);
	const float AdditionalCm = bFound ? (Additional * 100.f) : 0.f;

	const float MultiplierDelta = GetSourceAttributeValue(UFP_AttributeSet::GetAreaOfEffectMulitplierAttribute(), bFound);
	const float AreaScale = FMath::Max(1.f + (bFound ? MultiplierDelta : 0.f), 0.0001f);

	// Circle area ∝ r² → scale r by sqrt(1+M) to grow area proportionally.
	return (BaseRadius + AdditionalCm) * FMath::Sqrt(AreaScale);
}

void UFP_DamageGameplayAbility::GetEffectiveConeParams(float BaseLength, float BaseHalfAngleDeg,
                                                        float& OutLength, float& OutHalfAngleDeg) const
{
	bool bFound = false;
	const float Additional   = GetSourceAttributeValue(UFP_AttributeSet::GetAreaOfEffectAdditionalRadiusAttribute(), bFound);
	const float AdditionalCm = bFound ? (Additional * 100.f) : 0.f;

	const float MultiplierDelta = GetSourceAttributeValue(UFP_AttributeSet::GetAreaOfEffectMulitplierAttribute(), bFound);
	const float AreaScale = FMath::Max(1.f + (bFound ? MultiplierDelta : 0.f), 0.0001f);

	// 2D sector area ∝ L² × HalfAngle_rad; proportional scaling (L→kL, A→kA) gives k³ = (1+M).
	const float k = FMath::Pow(AreaScale, 1.f / 3.f);
	OutLength       = (BaseLength + AdditionalCm) * k;
	OutHalfAngleDeg = BaseHalfAngleDeg * k;
}

void UFP_DamageGameplayAbility::GetEffectiveRectParams(float BaseLength, float BaseWidth,
                                                        float& OutLength, float& OutWidth) const
{
	bool bFound = false;
	const float Additional   = GetSourceAttributeValue(UFP_AttributeSet::GetAreaOfEffectAdditionalRadiusAttribute(), bFound);
	const float AdditionalCm = bFound ? (Additional * 100.f) : 0.f;

	const float MultiplierDelta = GetSourceAttributeValue(UFP_AttributeSet::GetAreaOfEffectMulitplierAttribute(), bFound);
	const float AreaScale = FMath::Max(1.f + (bFound ? MultiplierDelta : 0.f), 0.0001f);

	// Rect area ∝ L × W → scale both by sqrt(1+M); additional radius extends length only.
	const float k = FMath::Sqrt(AreaScale);
	OutLength = (BaseLength + AdditionalCm) * k;
	OutWidth  = BaseWidth * k;
}

// ---------------------------------------------------------------------------

void UFP_DamageGameplayAbility::AssignRolledDamageMagnitudes(FGameplayEffectSpecHandle& DamageSpecHandle) const
{
	AppendSkillModifierTagsToDamageSpec(DamageSpecHandle);

	for (const TTuple<FGameplayTag, FDamageRange>& Pair : DamageTypes)
	{
		
		const float MinScaledDamage = Pair.Value.DamageMin.GetValueAtLevel(GetAbilityLevel());
		const float MaxScaledDamage = Pair.Value.DamageMax.GetValueAtLevel(GetAbilityLevel());
		const float LowerBoundDamage = FMath::Min(MinScaledDamage, MaxScaledDamage);
		const float UpperBoundDamage = FMath::Max(MinScaledDamage, MaxScaledDamage);
		const float ScaledDamage = FMath::FRandRange(LowerBoundDamage, UpperBoundDamage);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
	}
}

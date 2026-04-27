#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "Engine/Engine.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Player/FP_PlayerState.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"

void UFP_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!bDebugActivation) return;

	const FGameplayAbilitySpec* Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (!Spec) return;

	// Resolve skill tag from asset tags
	FGameplayTag SkillTag;
	if (Spec->Ability)
	{
		for (const FGameplayTag& Tag : Spec->Ability->GetAssetTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Skills"))))
			{
				SkillTag = Tag;
				break;
			}
		}
	}

	const int32 SpecLevel = Spec->Level;

	// Look up skill name and XP from PlayerState
	FName SkillName = NAME_None;
	int32 CurrentXP = 0;

	if (const APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
	{
		if (const AFP_PlayerState* PS = Pawn->GetPlayerState<AFP_PlayerState>())
		{
			if (SkillTag.IsValid())
				CurrentXP = PS->GetSkillXP(SkillTag);

			if (const UFP_SkillLibrary* Lib = PS->GetSkillLibrary())
			{
				const FFP_AbilityEntry Entry = Lib->FindAbilityEntryForTag(SkillTag, false);
				if (Entry.SkillTag.IsValid())
					SkillName = Entry.SkillName;
			}
		}
	}

	const FString Msg = FString::Printf(TEXT("[Skill] %s | Level %d | XP %d"),
		SkillName != NAME_None ? *SkillName.ToString() : *GetName(),
		SpecLevel,
		CurrentXP);

	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, Msg);
}

bool UFP_GameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (bAllowNegativeCost)
	{
		return true;
	}
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

UAbilitySystemComponent* UFP_GameplayAbility::GetSourceASC() const
{
	return GetAbilitySystemComponentFromActorInfo();
}

float UFP_GameplayAbility::GetSourceAttributeValue(const FGameplayAttribute& Attribute, bool& bFound) const
{
	bFound = false;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !Attribute.IsValid())
	{
		return 0.f;
	}

	bFound = true;
	return ASC->GetNumericAttribute(Attribute);
}

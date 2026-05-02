// Copyright JG

#include "SkillTree/FP_SkillTreeNodeEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "Player/FP_PlayerState.h"

// ---------------------------------------------------------------------------

static UAbilitySystemComponent* GetASC(APlayerController* PC)
{
	if (!PC) return nullptr;
	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn());
	return ASI ? ASI->GetAbilitySystemComponent() : nullptr;
}

// ---------------------------------------------------------------------------
// FFP_NodeEffect_StatModifier
// ---------------------------------------------------------------------------

void FFP_NodeEffect_StatModifier::OnAllocate(APlayerController* PC)
{
	UAbilitySystemComponent* ASC = GetASC(PC);
	if (!ASC || !AttributeTag.IsValid()) return;

	// Resolve attribute from tag via the attribute set's tag map
	const UFP_AttributeSet* AS = ASC->GetSet<UFP_AttributeSet>();
	if (!AS) return;

	const TStaticFuncPtr<FGameplayAttribute()>* FuncPtr = AS->TagsToAttributes.Find(AttributeTag);
	if (!FuncPtr) return;

	const FGameplayAttribute Attribute = (*FuncPtr)();

	// Build a dynamic infinite GE with a single modifier — no class asset required
	UGameplayEffect* DynamicGE = NewObject<UGameplayEffect>(GetTransientPackage(), NAME_None);
	DynamicGE->DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo ModInfo;
	ModInfo.Attribute        = Attribute;
	ModInfo.ModifierOp       = ModifierOp;
	ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Magnitude));
	DynamicGE->Modifiers.Add(ModInfo);

	const FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	const FGameplayEffectSpec Spec(DynamicGE, Context, 1.f);
	ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(Spec);
}

void FFP_NodeEffect_StatModifier::OnDeallocate(APlayerController* PC)
{
	if (!ActiveHandle.IsValid()) return;

	UAbilitySystemComponent* ASC = GetASC(PC);
	if (ASC) ASC->RemoveActiveGameplayEffect(ActiveHandle);
	ActiveHandle = FActiveGameplayEffectHandle();
}

// ---------------------------------------------------------------------------
// FFP_NodeEffect_GrantSkill
// ---------------------------------------------------------------------------

void FFP_NodeEffect_GrantSkill::OnAllocate(APlayerController* PC)
{
	if (!PC || !SkillTag.IsValid()) return;

	AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>();
	if (!PS) return;

	const UFP_SkillLibrary* Library = PS->GetSkillLibrary();
	if (!Library) return;

	const FFP_AbilityEntry Entry = Library->FindAbilityEntryForTag(SkillTag, true);
	if (!Entry.SkillAbility) return;

	UFP_AbilitySystemComponent* ASC = Cast<UFP_AbilitySystemComponent>(PS->GetAbilitySystemComponent());
	if (!ASC) return;

	GrantedHandle = ASC->GrantItemSkill(Entry.SkillAbility, PS->GetSkillLevel(SkillTag));
	PS->AddGrantedSkill(SkillTag);
}

void FFP_NodeEffect_GrantSkill::OnDeallocate(APlayerController* PC)
{
	AFP_PlayerState* PS = PC ? PC->GetPlayerState<AFP_PlayerState>() : nullptr;
	UFP_AbilitySystemComponent* ASC = PS
		? Cast<UFP_AbilitySystemComponent>(PS->GetAbilitySystemComponent())
		: nullptr;

	if (ASC && GrantedHandle.IsValid())
		ASC->RevokeItemSkill(GrantedHandle);

	GrantedHandle = FGameplayAbilitySpecHandle();

	if (PS && SkillTag.IsValid())
		PS->RemoveGrantedSkill(SkillTag);
}

// ---------------------------------------------------------------------------
// FFP_NodeEffect_Flag
// ---------------------------------------------------------------------------

void FFP_NodeEffect_Flag::OnAllocate(APlayerController* PC)
{
	UAbilitySystemComponent* ASC = GetASC(PC);
	if (ASC && FlagTag.IsValid())
		ASC->AddLooseGameplayTag(FlagTag);
}

void FFP_NodeEffect_Flag::OnDeallocate(APlayerController* PC)
{
	UAbilitySystemComponent* ASC = GetASC(PC);
	if (ASC && FlagTag.IsValid())
		ASC->RemoveLooseGameplayTag(FlagTag);
}

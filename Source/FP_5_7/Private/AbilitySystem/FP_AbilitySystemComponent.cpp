// Copyright JG


#include "AbilitySystem/FP_AbilitySystemComponent.h"

#include "FP_GameplayTags.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "Interaction/FP_PlayerInterface.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "Player/FP_PlayerState.h"
#include "GameplayEffect.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Interaction/FP_CombatInterface.h"
#include "AbilitySystem/Effects/FP_GE_Blur.h"

void UFP_AbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UFP_AbilitySystemComponent::ClientEffectApplied);

	// Pending deferred damage follows changes to the DeferredDamageTime stat (bind once).
	if (!DeferredDamageTimeChangedHandle.IsValid())
	{
		DeferredDamageTimeChangedHandle = GetGameplayAttributeValueChangeDelegate(
			UFP_AttributeSet::GetDeferredDamageTimeAttribute()).AddUObject(
				this, &UFP_AbilitySystemComponent::OnDeferredDamageTimeChanged);
	}

	RegisterFlagEffects();
}

void UFP_AbilitySystemComponent::RegisterFlagEffects()
{
	if (!FlagEffectClasses.IsEmpty())
	{
		return; // bind once
	}
	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();
	FlagEffectClasses.Add(Tags.Flag_SkillTree_Blur, UFP_GE_Blur::StaticClass());

	for (const TPair<FGameplayTag, TSubclassOf<UGameplayEffect>>& Pair : FlagEffectClasses)
	{
		RegisterGameplayTagEvent(Pair.Key, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &UFP_AbilitySystemComponent::OnFlagEffectTagChanged);
		// Flag may already be present (e.g. tree allocated before the actor info was set).
		if (GetTagCount(Pair.Key) > 0)
		{
			OnFlagEffectTagChanged(Pair.Key, GetTagCount(Pair.Key));
		}
	}
}

void UFP_AbilitySystemComponent::OnFlagEffectTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (!IsOwnerActorAuthoritative())
	{
		return; // the active effect replicates to clients
	}
	const TSubclassOf<UGameplayEffect>* EffectClass = FlagEffectClasses.Find(Tag);
	if (!EffectClass || !*EffectClass)
	{
		return;
	}
	FActiveGameplayEffectHandle& Handle = ActiveFlagEffects.FindOrAdd(Tag);
	if (NewCount > 0 && !Handle.IsValid())
	{
		FGameplayEffectContextHandle Context = MakeEffectContext();
		Context.AddSourceObject(GetOwner());
		const FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(*EffectClass, 1.f, Context);
		if (Spec.IsValid())
		{
			Handle = ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
	else if (NewCount <= 0 && Handle.IsValid())
	{
		RemoveActiveGameplayEffect(Handle);
		Handle.Invalidate();
	}
}

void UFP_AbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		if (!AbilityClass) continue;
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UFP_GameplayAbility* AuraAbility = Cast<UFP_GameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FFP_GameplayTags::Get().Skills_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast(this);
}

void UFP_AbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		if (!AbilityClass) continue;
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FFP_GameplayTags::Get().Skills_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UFP_AbilitySystemComponent::AddInputTagToSkill(const FGameplayTag& SkillTag, const FGameplayTag& InputTag)
{
	if (!SkillTag.IsValid() || !InputTag.IsValid()) return;

	FScopedAbilityListLock Lock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!GetAbilityTagFromSpec(Spec).MatchesTagExact(SkillTag)) continue;

		FGameplayTagContainer& DynTags = Spec.GetDynamicSpecSourceTags();
		if (!DynTags.HasTagExact(InputTag))
		{
			DynTags.AddTag(InputTag);
			MarkAbilitySpecDirty(Spec);
		}
		break;
	}
}

void UFP_AbilitySystemComponent::RemoveInputTagFromSkill(const FGameplayTag& SkillTag, const FGameplayTag& InputTag)
{
	if (!SkillTag.IsValid() || !InputTag.IsValid()) return;

	FScopedAbilityListLock Lock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!GetAbilityTagFromSpec(Spec).MatchesTagExact(SkillTag)) continue;

		FGameplayTagContainer& DynTags = Spec.GetDynamicSpecSourceTags();
		if (DynTags.HasTagExact(InputTag))
		{
			DynTags.RemoveTag(InputTag);
			MarkAbilitySpecDirty(Spec);
		}
		break;
	}
}

void UFP_AbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UFP_AbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

FGameplayAbilitySpecHandle UFP_AbilitySystemComponent::GrantItemSkill(TSubclassOf<UFP_GameplayAbility> AbilityClass, int32 Level)
{
	if (!AbilityClass) return FGameplayAbilitySpecHandle();
	FGameplayAbilitySpec Spec(AbilityClass, FMath::Max(1, Level));
	Spec.GetDynamicSpecSourceTags().AddTag(FFP_GameplayTags::Get().Skills_Status_Equipped);
	return GiveAbility(Spec);
}

void UFP_AbilitySystemComponent::RevokeItemSkill(FGameplayAbilitySpecHandle Handle)
{
	if (Handle.IsValid())
		ClearAbility(Handle);
}

void UFP_AbilitySystemComponent::ForEachAbility(const FForEachAbilty& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UFP_AbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		const FGameplayTag SkillTag = FGameplayTag::RequestGameplayTag(FName("Skill"));
		const FGameplayTag SkillsTag = FGameplayTag::RequestGameplayTag(FName("Skills"));
		const FGameplayTagContainer& AssetTags = AbilitySpec.Ability->GetAssetTags();
		for (const FGameplayTag& Tag : AssetTags)

		{
			
			if (Tag.MatchesTag(SkillTag) || Tag.MatchesTag(SkillsTag))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UFP_AbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void UFP_AbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast(this);
	}
}


void UFP_AbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UFP_PlayerInterface>())
	{
		if (IFP_PlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UFP_AbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	if (const UFP_AttributeSet* AS = GetSet<UFP_AttributeSet>())
	{
		if (const TStaticFuncPtr<FGameplayAttribute()>* FuncPtr = AS->TagsToAttributes.Find(AttributeTag))
		{
			const FGameplayAttribute Attribute = (*FuncPtr)();
			SetNumericAttributeBase(Attribute, GetNumericAttributeBase(Attribute) + 1.f);
		}
	}

	if (GetAvatarActor()->Implements<UFP_PlayerInterface>())
	{
		IFP_PlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}

	if (AFP_PlayerState* PS = Cast<AFP_PlayerState>(GetOwner()))
	{
		PS->AddToPassivePoints(AttributeTag, 1);
	}
}

void UFP_AbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                    const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}

// ---------------------------------------------------------------------------
// Deferred damage
// ---------------------------------------------------------------------------

void UFP_AbilitySystemComponent::AddDeferredDamage(float Amount, float Duration, UAbilitySystemComponent* SourceASC)
{
	if (Amount <= 0.f || !IsOwnerActorAuthoritative())
	{
		return;
	}

	FFP_DeferredDamageInstance& Instance = DeferredDamageInstances.AddDefaulted_GetRef();
	Instance.RemainingDamage = Amount;
	Instance.RemainingTime = FMath::Max(Duration, 0.f);
	Instance.SourceASC = SourceASC;
	Instance.bRecoup = RecoupsDeferredDamage(this);

	UWorld* World = GetWorld();
	if (World && !World->GetTimerManager().IsTimerActive(DeferredDamageTimer))
	{
		LastDeferredTickTime = World->GetTimeSeconds();
		World->GetTimerManager().SetTimer(DeferredDamageTimer, this, &UFP_AbilitySystemComponent::TickDeferredDamage,
			FMath::Max(DeferredDamageTickInterval, 0.02f), true);
	}
}

void UFP_AbilitySystemComponent::OnDeferredDamageTimeChanged(const FOnAttributeChangeData& Data)
{
	if (!IsOwnerActorAuthoritative() || DeferredDamageInstances.IsEmpty())
	{
		return;
	}
	// Instances only exist if the time was > 0 when they were created; with no meaningful
	// old value there is nothing to scale against.
	if (Data.OldValue <= KINDA_SMALL_NUMBER)
	{
		return;
	}
	// Proportional: 10s -> 5s halves every instance's remaining time (6s left -> 3s left).
	ScaleDeferredDamageRemainingTime(FMath::Max(Data.NewValue, 0.f) / Data.OldValue);
}

void UFP_AbilitySystemComponent::SetDeferredDamageRemainingTime(float NewRemainingTime)
{
	for (FFP_DeferredDamageInstance& Instance : DeferredDamageInstances)
	{
		Instance.RemainingTime = FMath::Max(NewRemainingTime, 0.f);
	}
}

void UFP_AbilitySystemComponent::ScaleDeferredDamageRemainingTime(float Multiplier)
{
	for (FFP_DeferredDamageInstance& Instance : DeferredDamageInstances)
	{
		Instance.RemainingTime = FMath::Max(Instance.RemainingTime * Multiplier, 0.f);
	}
}

float UFP_AbilitySystemComponent::GetIncreasedDamageTaken() const
{
	float Increased = 0.f;
	if (DeferredDamageInstances.Num() > 0
		&& HasMatchingGameplayTag(FFP_GameplayTags::Get().Flag_SkillTree_IncreasedDamageTaken_WhileDeferring))
	{
		Increased += BideDamageTakenWhileDeferring;
	}
	return Increased;
}

float UFP_AbilitySystemComponent::GetPendingDeferredDamage() const
{
	float Total = 0.f;
	for (const FFP_DeferredDamageInstance& Instance : DeferredDamageInstances)
	{
		if (!Instance.bRecoup)
		{
			Total += Instance.RemainingDamage;
		}
	}
	return Total;
}

float UFP_AbilitySystemComponent::GetPendingRecoup() const
{
	float Total = 0.f;
	for (const FFP_DeferredDamageInstance& Instance : DeferredDamageInstances)
	{
		if (Instance.bRecoup)
		{
			Total += Instance.RemainingDamage;
		}
	}
	return Total;
}

bool UFP_AbilitySystemComponent::IsAvatarMoving(const UAbilitySystemComponent* ASC)
{
	const AActor* Avatar = ASC ? ASC->GetAvatarActor() : nullptr;
	return Avatar && Avatar->GetVelocity().Size2D() > PerpetualMotionMovingSpeed;
}

float UFP_AbilitySystemComponent::GetPerpetualMotionDamageMultiplier(const UAbilitySystemComponent* SourceASC)
{
	if (!SourceASC || !SourceASC->HasMatchingGameplayTag(FFP_GameplayTags::Get().Flag_SkillTree_PerpetualMotion))
	{
		return 1.f;
	}
	return IsAvatarMoving(SourceASC) ? 1.f + PerpetualMotionMovingMore : 1.f - PerpetualMotionStationaryLess;
}

float UFP_AbilitySystemComponent::ApplyPerpetualMotionDodge(const UAbilitySystemComponent* TargetASC, float DodgeChance)
{
	if (!TargetASC || !TargetASC->HasMatchingGameplayTag(FFP_GameplayTags::Get().Flag_SkillTree_PerpetualMotion))
	{
		return DodgeChance;
	}
	return IsAvatarMoving(TargetASC) ? DodgeChance + PerpetualMotionMovingDodge : 0.f;
}

bool UFP_AbilitySystemComponent::RecoupsDeferredDamage(const UAbilitySystemComponent* ASC)
{
	return ASC && ASC->HasMatchingGameplayTag(FFP_GameplayTags::Get().Flag_SkillTree_RecoupInsteadOfDefer);
}

void UFP_AbilitySystemComponent::ClearDeferredDamage()
{
	DeferredDamageInstances.Reset();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DeferredDamageTimer);
	}
}

void UFP_AbilitySystemComponent::TickDeferredDamage()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	const double Now = World->GetTimeSeconds();
	const float DeltaTime = static_cast<float>(Now - LastDeferredTickTime);
	LastDeferredTickTime = Now;

	AActor* Avatar = GetAvatarActor();
	if (!Avatar || (Avatar->Implements<UFP_CombatInterface>() && IFP_CombatInterface::Execute_IsDead(Avatar)))
	{
		ClearDeferredDamage();
		return;
	}

	// Pay out each instance's share for this tick, grouped by source so kill credit stays correct.
	// Recoup (Restorative Nanites) instances pay out as healing instead.
	TMap<TWeakObjectPtr<UAbilitySystemComponent>, float> DamageBySource;
	float RecoupThisTick = 0.f;
	for (int32 i = DeferredDamageInstances.Num() - 1; i >= 0; --i)
	{
		FFP_DeferredDamageInstance& Instance = DeferredDamageInstances[i];
		const float Payout = (Instance.RemainingTime <= DeltaTime)
			? Instance.RemainingDamage
			: Instance.RemainingDamage * (DeltaTime / Instance.RemainingTime);

		Instance.RemainingDamage -= Payout;
		Instance.RemainingTime = FMath::Max(Instance.RemainingTime - DeltaTime, 0.f);
		if (Instance.bRecoup)
		{
			RecoupThisTick += Payout;
		}
		else
		{
			DamageBySource.FindOrAdd(Instance.SourceASC) += Payout;
		}

		if (Instance.RemainingDamage <= KINDA_SMALL_NUMBER)
		{
			DeferredDamageInstances.RemoveAtSwap(i);
		}
	}

	if (DeferredDamageInstances.IsEmpty())
	{
		World->GetTimerManager().ClearTimer(DeferredDamageTimer);
	}

	// Heal before damage, so a tick that does both can't kill through life that was being restored.
	if (RecoupThisTick > 0.f)
	{
		ApplyRecoupTick(RecoupThisTick);
	}
	for (const TPair<TWeakObjectPtr<UAbilitySystemComponent>, float>& Pair : DamageBySource)
	{
		if (Pair.Value > 0.f)
		{
			ApplyDeferredDamageTick(Pair.Value, Pair.Key.Get());
		}
	}
}

void UFP_AbilitySystemComponent::ApplyRecoupTick(float Amount)
{
	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();

	if (!RecoupTickEffect)
	{
		RecoupTickEffect = NewObject<UGameplayEffect>(this, TEXT("GE_DeferredRecoupTick"), RF_Transient);
		RecoupTickEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

		FSetByCallerFloat SetByCaller;
		SetByCaller.DataTag = Tags.SetByCaller_DeferredDamagePerTick;

		FGameplayModifierInfo Mod;
		Mod.Attribute = UFP_AttributeSet::GetHitPointsAttribute();   // clamped to MaxHitPoints by the attribute set
		Mod.ModifierOp = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		RecoupTickEffect->Modifiers.Add(Mod);
	}

	FGameplayEffectSpec Spec(RecoupTickEffect, MakeEffectContext(), 1.f);
	Spec.SetSetByCallerMagnitude(Tags.SetByCaller_DeferredDamagePerTick, Amount);
	Spec.AddDynamicAssetTag(Tags.Effects_DeferredDamageTick);
	ApplyGameplayEffectSpecToSelf(Spec);
}

void UFP_AbilitySystemComponent::ApplyDeferredDamageTick(float Amount, UAbilitySystemComponent* SourceASC)
{
	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();

	if (!DeferredDamageTickEffect)
	{
		DeferredDamageTickEffect = NewObject<UGameplayEffect>(this, TEXT("GE_DeferredDamageTick"), RF_Transient);
		DeferredDamageTickEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

		FSetByCallerFloat SetByCaller;
		SetByCaller.DataTag = Tags.SetByCaller_DeferredDamagePerTick;

		FGameplayModifierInfo Mod;
		Mod.Attribute = UFP_AttributeSet::GetIncomingDamageAttribute();
		Mod.ModifierOp = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		DeferredDamageTickEffect->Modifiers.Add(Mod);
	}

	// Instigator = original attacker when still around, so a lethal drip credits them.
	UAbilitySystemComponent* ContextSource = SourceASC ? SourceASC : this;
	const FGameplayEffectContextHandle Context = ContextSource->MakeEffectContext();

	FGameplayEffectSpec Spec(DeferredDamageTickEffect, Context, 1.f);
	Spec.SetSetByCallerMagnitude(Tags.SetByCaller_DeferredDamagePerTick, Amount);
	Spec.AddDynamicAssetTag(Tags.Effects_DeferredDamageTick);
	ApplyGameplayEffectSpecToSelf(Spec);
}

UGameplayEffect* UFP_AbilitySystemComponent::GetSkillCostEffect()
{
	if (!SkillCostEffect)
	{
		const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();
		SkillCostEffect = NewObject<UGameplayEffect>(this, TEXT("GE_SkillCost"), RF_Transient);
		SkillCostEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

		auto AddSetByCallerMod = [this](const FGameplayAttribute& Attribute, const FGameplayTag& DataTag)
		{
			FSetByCallerFloat SetByCaller;
			SetByCaller.DataTag = DataTag;

			FGameplayModifierInfo Mod;
			Mod.Attribute = Attribute;
			Mod.ModifierOp = EGameplayModOp::Additive;
			Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
			SkillCostEffect->Modifiers.Add(Mod);
		};
		AddSetByCallerMod(UFP_AttributeSet::GetHeatAttribute(), Tags.SetByCaller_SkillCost_Heat);
		AddSetByCallerMod(UFP_AttributeSet::GetHitPointsAttribute(), Tags.SetByCaller_SkillCost_HitPoints);
	}
	return SkillCostEffect;
}

void UFP_AbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearDeferredDamage();
	Super::EndPlay(EndPlayReason);
}

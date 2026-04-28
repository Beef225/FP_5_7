// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_PlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UFP_LevelUpInfo;
class UFP_SkillLibrary;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPassivePointsChanged, FGameplayTag /*AttributeTag*/, int32 /*Points*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillXPChanged,     FGameplayTag /*SkillTag*/, int32 /*NewXP*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillLevelChanged,  FGameplayTag /*SkillTag*/, int32 /*NewLevel*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillPointsChanged, FGameplayTag /*SkillTag*/, int32 /*UnspentPoints*/)
/**
 * 
 */
UCLASS()
class FP_5_7_API AFP_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AFP_PlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnPlayerStatChanged OnLevelChangedDelegate;
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPassivePointsChanged OnPassivePointsChangedDelegate;
	FOnSkillXPChanged     OnSkillXPChangedDelegate;
	FOnSkillLevelChanged  OnSkillLevelChangedDelegate;
	FOnSkillPointsChanged OnSkillPointsChangedDelegate;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetMightPassivePoints() const { return MightPassivePoints; }
	FORCEINLINE int32 GetResonancePassivePoints() const { return ResonancePassivePoints; }
	FORCEINLINE int32 GetAgilityPassivePoints() const { return AgilityPassivePoints; }
	FORCEINLINE int32 GetFortitudePassivePoints() const { return FortitudePassivePoints; }

	/** MonsterLevel = -1 means no penalty (e.g. quest rewards). */
	void AddSkillXP(int32 InXP, int32 MonsterLevel = -1);
	void LoadSkillState(const TMap<FGameplayTag, int32>& InXP, const TMap<FGameplayTag, int32>& InLevels,
	                    const TMap<FGameplayTag, int32>& InPoints, const TMap<FGameplayTag, FGameplayTag>& InInputTags);

	/** Seeds GrantedSkillTags from any entry in the SkillLibrary asset with bGranted == true.
	 *  Call once for a new character before the first save. */
	void InitGrantedSkillsFromLibrary();

	/** Call when any source (item, quest, level-up, etc.) grants a skill at runtime. */
	void AddGrantedSkill(const FGameplayTag& SkillTag);

	/** Call when a skill is revoked (item unequipped, etc.). */
	void RemoveGrantedSkill(const FGameplayTag& SkillTag);

	FORCEINLINE bool IsSkillGranted(const FGameplayTag& SkillTag) const { return GrantedSkillTags.Contains(SkillTag); }
	TArray<FGameplayTag> GetGrantedSkillTagsArray() const { return GrantedSkillTags.Array(); }
	void LoadGrantedSkills(const TArray<FGameplayTag>& InTags) { GrantedSkillTags = TSet<FGameplayTag>(InTags); }

	FORCEINLINE int32 GetSkillXP(const FGameplayTag& SkillTag) const     { const int32* Found = SkillXP.Find(SkillTag);           return Found ? *Found : 0; }
	FORCEINLINE int32 GetSkillLevel(const FGameplayTag& SkillTag) const  { const int32* Found = SkillLevel.Find(SkillTag);         return Found ? *Found : 1; }
	FORCEINLINE int32 GetSkillPoints(const FGameplayTag& SkillTag) const { const int32* Found = UnspentSkillPoints.Find(SkillTag); return Found ? *Found : 0; }
	FORCEINLINE FGameplayTag GetSkillInputTag(const FGameplayTag& SkillTag) const { const FGameplayTag* Found = SkillInputTags.Find(SkillTag); return Found ? *Found : FGameplayTag(); }

	void SetSkillInputTag(const FGameplayTag& SkillTag, const FGameplayTag& InputTag) { SkillInputTags.Add(SkillTag, InputTag); }

	const TMap<FGameplayTag, int32>& GetSkillXPMap() const           { return SkillXP; }
	const TMap<FGameplayTag, int32>& GetSkillLevelMap() const        { return SkillLevel; }
	const TMap<FGameplayTag, int32>& GetSkillUnspentPointsMap() const { return UnspentSkillPoints; }
	const TMap<FGameplayTag, FGameplayTag>& GetSkillInputTagMap() const { return SkillInputTags; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	UFP_SkillLibrary* GetSkillLibrary() const { return SkillLibrary; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	EWeaponGripStyle GetGripStance() const { return GripStance; }

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetGripStance(EWeaponGripStyle InGripStance);
	
	void AddToXP(int32 InXP);
	void AddToLevel(int32 InLevel);
	void AddToAttributePoints(int32 InPoints);
	void AddToPassivePoints(const FGameplayTag& AttributeTag, int32 InPoints);

	void SetXP(int32 InXP);
	void SetLevel(int32 InLevel);

	/**
	 * Restores attribute point allocation from a save record.
	 * Sets the unspent pool and per-attribute passive point counters,
	 * then adds the saved points as base value increases on top of the
	 * defaults already applied by InitializeDefaultAttributes.
	 * Call this from OnPossess, after InitializeDefaultAttributes has run.
	 */
	void LoadAllocatedPoints(int32 InUnspent, int32 InMight, int32 InResonance, int32 InAgility, int32 InFortitude);
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFP_LevelUpInfo> LevelUpInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UFP_SkillLibrary> SkillLibrary;

	
protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
private:

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_MightPassivePoints)
	int32 MightPassivePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_ResonancePassivePoints)
	int32 ResonancePassivePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AgilityPassivePoints)
	int32 AgilityPassivePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_FortitudePassivePoints)
	int32 FortitudePassivePoints = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);

	UFUNCTION()
	void OnRep_MightPassivePoints(int32 OldPoints);

	UFUNCTION()
	void OnRep_ResonancePassivePoints(int32 OldPoints);

	UFUNCTION()
	void OnRep_AgilityPassivePoints(int32 OldPoints);

	UFUNCTION()
	void OnRep_FortitudePassivePoints(int32 OldPoints);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_GripStance, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	EWeaponGripStyle GripStance = EWeaponGripStyle::Unarmed;

	UFUNCTION()
	void OnRep_GripStance(EWeaponGripStyle OldGripStance);

	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, int32> SkillXP;

	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, int32> SkillLevel;

	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, int32> UnspentSkillPoints;

	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, FGameplayTag> SkillInputTags;

	/** Skill XP: one-directional — only penalises when character < monster (rushing high content).
	 *  Overlevelled character vs low monster = no penalty, enabling skill catch-up. */
	static float ComputeSkillXPMultiplier(int32 PlayerLevel, int32 MonsterLevel);

	/** Character XP: bidirectional — penalises both farming low AND rushing high content. */
	static float ComputeCharacterXPMultiplier(int32 PlayerLevel, int32 MonsterLevel);

	/** Runtime source of truth for which skills this character currently has.
	 *  Seeded from SkillLibrary asset bGranted defaults on new character creation.
	 *  Modified at runtime by AddGrantedSkill / RemoveGrantedSkill from any grant source. */
	UPROPERTY(VisibleAnywhere)
	TSet<FGameplayTag> GrantedSkillTags;

};

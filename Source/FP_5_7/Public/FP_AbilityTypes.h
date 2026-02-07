#pragma once

#include "GameplayEffectTypes.h"
#include "FP_AbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FFP_GameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit()  const { return bIsBlockedHit; }
	bool IsDodgedHit()   const { return bIsDodgedHit; }

	void SetIsCriticalHit(bool bIn) { bIsCriticalHit = bIn; }
	void SetIsBlockedHit (bool bIn) { bIsBlockedHit  = bIn; }
	void SetIsDodgedHit  (bool bIn) { bIsDodgedHit   = bIn; }

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FFP_GameplayEffectContext::StaticStruct();
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FFP_GameplayEffectContext* Duplicate() const
	{
		FFP_GameplayEffectContext* NewContext = new FFP_GameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

protected:
	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bIsDodgedHit = false;
};

template<>
struct TStructOpsTypeTraits<FFP_GameplayEffectContext> : public TStructOpsTypeTraitsBase2<FFP_GameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

// Copyright JG


#include "FP_AbilityTypes.h"


bool FFP_GameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;

	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())               RepBits |= 1 << 0;
		if (bReplicateEffectCauser && EffectCauser.IsValid())           RepBits |= 1 << 1;
		if (AbilityCDO.IsValid())                                       RepBits |= 1 << 2;
		if (bReplicateSourceObject && SourceObject.IsValid())           RepBits |= 1 << 3;
		if (Actors.Num() > 0)                                           RepBits |= 1 << 4;
		if (HitResult.IsValid())                                        RepBits |= 1 << 5;
		if (bHasWorldOrigin)                                            RepBits |= 1 << 6;

		// Custom flags (bit == truth)
		if (bIsBlockedHit)                                              RepBits |= 1 << 7;
		if (bIsCriticalHit)                                             RepBits |= 1 << 8;
		if (bIsDodgedHit)                                               RepBits |= 1 << 9;
	}

	// Update count if you add more bits later
	Ar.SerializeBits(&RepBits, 10);

	if (RepBits & (1 << 0)) Ar << Instigator;
	if (RepBits & (1 << 1)) Ar << EffectCauser;
	if (RepBits & (1 << 2)) Ar << AbilityCDO;
	if (RepBits & (1 << 3)) Ar << SourceObject;

	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}

	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading() && !HitResult.IsValid())
		{
			HitResult = TSharedPtr<FHitResult>(new FHitResult());
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}

	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}

	// Derive bools from bits (no extra serialization)
	bIsBlockedHit  = (RepBits & (1 << 7)) != 0;
	bIsCriticalHit = (RepBits & (1 << 8)) != 0;
	bIsDodgedHit   = (RepBits & (1 << 9)) != 0;

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get());
	}

	bOutSuccess = true;
	return true;
}

// Copyright JG

#include "Animation/Data/FP_UpperBodyStanceSet.h"

UAnimSequence* UFP_UpperBodyStanceSet::GetSequenceForGrip(EWeaponGripStyle Grip) const
{
	if (const TObjectPtr<UAnimSequence>* Found = GripStyleSequences.Find(Grip))
	{
		return Found->Get();
	}

	return DefaultStanceSequence.Get();
}

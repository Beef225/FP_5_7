// Copyright JG

#include "Animation/FP_PreviewAnimInstance.h"
#include "Preview/FP_CharacterPreviewActor.h"
#include "Animation/Data/FP_UpperBodyStanceSet.h"

void UFP_PreviewAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningPreviewActor = Cast<AFP_CharacterPreviewActor>(GetOwningActor());
}

void UFP_PreviewAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningPreviewActor)
	{
		OwningPreviewActor = Cast<AFP_CharacterPreviewActor>(GetOwningActor());
		if (!OwningPreviewActor) return;
	}

	GripStance = OwningPreviewActor->GetPreviewGripStance();

	if (UFP_UpperBodyStanceSet* Stances = OwningPreviewActor->GetStanceSet())
	{
		UpperBodySequence = Stances->GetSequenceForGrip(GripStance);
		bHasUpperBody     = IsValid(UpperBodySequence);
	}
	else
	{
		UpperBodySequence = nullptr;
		bHasUpperBody     = false;
	}
}

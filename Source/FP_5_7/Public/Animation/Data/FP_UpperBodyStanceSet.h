// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Libraries/FP_EnumDefs.h"      // EWeaponGripStyle
#include "Animation/AnimSequence.h"
#include "FP_UpperBodyStanceSet.generated.h"

UCLASS(BlueprintType)
class FP_5_7_API UFP_UpperBodyStanceSet : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Optional fallback stance if Grip is not found (or if you want to treat Unarmed as "no overlay"). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Upper Body")
	TObjectPtr<UAnimSequence> DefaultStanceSequence = nullptr;

	/** Grip Style → Upper body stance pose/idle (AnimSequence). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Upper Body")
	TMap<EWeaponGripStyle, TObjectPtr<UAnimSequence>> GripStyleSequences;

	/** Returns the stance sequence for a grip (falls back to DefaultStanceSequence). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Upper Body")
	UAnimSequence* GetSequenceForGrip(EWeaponGripStyle Grip) const;
};

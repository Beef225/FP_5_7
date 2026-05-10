// Copyright JG

#include "Animation/FP_PlayerAnimInstance.h"
#include "Characters/FP_PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UFP_PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningCharacter = Cast<AFP_PlayerCharacter>(TryGetPawnOwner());
}

void UFP_PlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningCharacter)
	{
		OwningCharacter = Cast<AFP_PlayerCharacter>(TryGetPawnOwner());
		if (!OwningCharacter) return;
	}

	USkeletalMeshComponent* WeaponMesh = OwningCharacter->GetLeftHandIKWeaponMesh();
	const FName SocketName             = OwningCharacter->GetLeftHandIKSocket();

	bUseLeftHandIK = IsValid(WeaponMesh) && !SocketName.IsNone();

	if (bUseLeftHandIK)
	{
		// Get the socket world position and convert to the character mesh's component space.
		// Two-Bone IK effector expects a component-space vector.
		const FVector SocketWorld      = WeaponMesh->GetSocketLocation(SocketName);
		const FTransform MeshTransform = OwningCharacter->GetMesh()->GetComponentTransform();
		LeftHandIKLocation             = MeshTransform.InverseTransformPosition(SocketWorld);
		LeftHandJointTargetLocation    = OwningCharacter->GetLeftHandJointTargetLocation();
	}
}

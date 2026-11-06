// Copyright JG

#include "Libraries/FP_WeaponFXLibrary.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/MeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "FP_GameplayTags.h"
#include "Actor/Skills/FP_EjectedCasing.h"
#include "Characters/FP_CharacterBase.h"
#include "Interaction/FP_CombatInterface.h"

UMeshComponent* UFP_WeaponFXLibrary::FindMeshWithSocket(AActor* Actor, FName SocketName)
{
	if (!Actor || SocketName.IsNone()) return nullptr;

	const ACharacter* Character = Cast<ACharacter>(Actor);
	const UMeshComponent* Body = Character ? Character->GetMesh() : nullptr;

	TArray<UMeshComponent*> Meshes;
	Actor->GetComponents<UMeshComponent>(Meshes);

	// Weapon meshes (anything but the body) win, so a body socket with the same name can't steal the muzzle.
	for (UMeshComponent* Mesh : Meshes)
	{
		if (Mesh && Mesh != Body && Mesh->DoesSocketExist(SocketName)) return Mesh;
	}
	if (Body && Body->DoesSocketExist(SocketName)) return const_cast<UMeshComponent*>(Body);
	return nullptr;
}

FName UFP_WeaponFXLibrary::GetMuzzleSocketName(const AActor* Shooter, const FFP_WeaponFireFX& FX)
{
	if (!FX.MuzzleSocketOverride.IsNone()) return FX.MuzzleSocketOverride;
	if (const AFP_CharacterBase* Character = Cast<AFP_CharacterBase>(Shooter)) return Character->GetWeaponTipSocketName();
	return NAME_None;
}

void UFP_WeaponFXLibrary::PlayWeaponFireFX(AActor* Shooter, const FFP_WeaponFireFX& FX)
{
	if (!Shooter) return;
	UWorld* World = Shooter->GetWorld();
	if (!World || World->GetNetMode() == NM_DedicatedServer) return;

	// ---- Muzzle flash + sound ---------------------------------------------------------------
	if (FX.MuzzleFlash || FX.FireSound)
	{
		const FName MuzzleSocket = GetMuzzleSocketName(Shooter, FX);
		UMeshComponent* WeaponMesh = FindMeshWithSocket(Shooter, MuzzleSocket);

		FVector MuzzleLocation;
		FRotator MuzzleRotation;
		if (WeaponMesh)
		{
			MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocket);
			MuzzleRotation = WeaponMesh->GetSocketRotation(MuzzleSocket);
		}
		else
		{
			MuzzleLocation = Shooter->Implements<UFP_CombatInterface>()
				? IFP_CombatInterface::Execute_GetCombatSocketLocation(Shooter, FFP_GameplayTags::Get().Montage_Skill_Weapon)
				: Shooter->GetActorLocation();
			MuzzleRotation = Shooter->GetActorRotation();
		}

		if (FX.MuzzleFlash)
		{
			if (WeaponMesh && FX.bAttachMuzzleFlash)
			{
				UNiagaraFunctionLibrary::SpawnSystemAttached(FX.MuzzleFlash, WeaponMesh, MuzzleSocket,
					FVector::ZeroVector, FRotator::ZeroRotator, FX.MuzzleFlashScale,
					EAttachLocation::SnapToTarget, /*bAutoDestroy=*/true, ENCPoolMethod::AutoRelease);
			}
			else
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, FX.MuzzleFlash, MuzzleLocation, MuzzleRotation,
					FX.MuzzleFlashScale, true, true, ENCPoolMethod::AutoRelease);
			}
		}

		if (FX.FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(Shooter, FX.FireSound, MuzzleLocation);
		}
	}

	// ---- Casing ------------------------------------------------------------------------------
	if (!FX.CasingClass) return;

	UMeshComponent* EjectMesh = FindMeshWithSocket(Shooter, FX.EjectSocketName);
	if (!EjectMesh)
	{
		UE_LOG(LogTemp, Verbose, TEXT("PlayWeaponFireFX: no mesh on %s has socket '%s' — no casing."),
			*Shooter->GetName(), *FX.EjectSocketName.ToString());
		return;
	}

	const FTransform SpawnTransform(EjectMesh->GetSocketRotation(FX.EjectSocketName),
	                                EjectMesh->GetSocketLocation(FX.EjectSocketName));

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = Shooter;
	AFP_EjectedCasing* Casing = World->SpawnActor<AFP_EjectedCasing>(FX.CasingClass, SpawnTransform, Params);
	if (!Casing) return;

	// Direction in the shooter's frame: X forward, Y right, Z world up.
	const FVector Forward = Shooter->GetActorForwardVector().GetSafeNormal2D();
	const FVector Right   = FVector::CrossProduct(FVector::UpVector, Forward);
	FVector Dir = Forward * FX.EjectDirection.X + Right * FX.EjectDirection.Y + FVector::UpVector * FX.EjectDirection.Z;
	if (!Dir.Normalize()) Dir = FVector::UpVector;
	if (FX.EjectConeHalfAngle > 0.f)
	{
		Dir = FMath::VRandCone(Dir, FMath::DegreesToRadians(FX.EjectConeHalfAngle));
	}

	const float Speed = FX.EjectSpeed * FMath::FRandRange(1.f - FX.EjectSpeedVariance, 1.f + FX.EjectSpeedVariance);
	FVector Velocity = Dir * Speed;
	if (FX.bInheritOwnerVelocity) Velocity += Shooter->GetVelocity();

	const FVector Spin = FMath::VRand() * FX.EjectSpinDegPerSec;
	Casing->Launch(Velocity, Spin);
}

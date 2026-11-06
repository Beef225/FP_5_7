// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FP_WeaponFXLibrary.generated.h"

class UNiagaraSystem;
class USoundBase;
class UMeshComponent;
class AFP_EjectedCasing;

/**
 * Per-shot cosmetic effects for a gun: muzzle flash, fire sound, ejected casing.
 * Lives on the skill (e.g. UFP_ProjectileSkill::FireFX) and is played by PlayWeaponFireFX.
 * Every field is optional — leave a field empty to skip that effect.
 */
USTRUCT(BlueprintType)
struct FFP_WeaponFireFX
{
	GENERATED_BODY()

	// ---- Muzzle ---------------------------------------------------------------------

	/** Spawned at the muzzle socket, using the socket's rotation (author the flash pointing +X). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Muzzle")
	TObjectPtr<UNiagaraSystem> MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Muzzle")
	FVector MuzzleFlashScale = FVector(1.f);

	/** Attach the flash to the weapon so it follows the gun; off = spawn in world space. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Muzzle")
	bool bAttachMuzzleFlash = true;

	/** Muzzle socket. None = the character's WeaponTipSocketName (same socket projectiles leave from). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Muzzle")
	FName MuzzleSocketOverride = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Muzzle")
	TObjectPtr<USoundBase> FireSound;

	// ---- Casing ---------------------------------------------------------------------

	/** Casing / debris to throw out per shot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Casing")
	TSubclassOf<AFP_EjectedCasing> CasingClass;

	/** Socket on the weapon mesh the casing spawns from (spawn uses the socket's rotation). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Casing")
	FName EjectSocketName = FName("EjectSocket");

	/** Throw direction relative to the CHARACTER: X forward, Y right, Z up. Default: up and to the right. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Casing")
	FVector EjectDirection = FVector(-0.2f, 1.f, 1.f);

	/** Throw speed (cm/s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Casing", meta=(ClampMin="0.0"))
	float EjectSpeed = 250.f;

	/** Random +/- fraction applied to EjectSpeed (0.2 = 80%-120%). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Casing", meta=(ClampMin="0.0", ClampMax="1.0"))
	float EjectSpeedVariance = 0.2f;

	/** Random cone (degrees) around EjectDirection. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Casing", meta=(ClampMin="0.0", ClampMax="90.0"))
	float EjectConeHalfAngle = 15.f;

	/** Tumble speed (deg/s) around a random axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Casing", meta=(ClampMin="0.0"))
	float EjectSpinDegPerSec = 900.f;

	/** Add the shooter's velocity so casings don't hang in the air behind a running character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Casing")
	bool bInheritOwnerVelocity = true;
};

UCLASS()
class FP_5_7_API UFP_WeaponFXLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Plays one shot's effects on Shooter's weapon: muzzle flash + fire sound at the muzzle socket,
	 * casing thrown from the eject socket. Local only (call it wherever the shot happens; skipped on
	 * dedicated servers). Falls back to the shooter's combat socket location if no weapon mesh has the socket.
	 */
	UFUNCTION(BlueprintCallable, Category="FP|WeaponFX")
	static void PlayWeaponFireFX(AActor* Shooter, const FFP_WeaponFireFX& FX);

	/** First mesh component on Actor with SocketName — weapon meshes first, the character body last. */
	UFUNCTION(BlueprintPure, Category="FP|WeaponFX")
	static UMeshComponent* FindMeshWithSocket(AActor* Actor, FName SocketName);

	/** The muzzle socket name PlayWeaponFireFX will use for Shooter. */
	UFUNCTION(BlueprintPure, Category="FP|WeaponFX")
	static FName GetMuzzleSocketName(const AActor* Shooter, const FFP_WeaponFireFX& FX);
};

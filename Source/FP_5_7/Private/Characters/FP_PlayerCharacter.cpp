// Copyright JG

#include "Characters/FP_PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "Player/FP_PlayerController.h"
#include "Abilitysystem\FP_AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/FP_PlayerState.h"
#include "AbilitySystem/Data/FP_LevelUpInfo.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/HUD/FP_HUD.h"

AFP_PlayerCharacter::AFP_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Movement settings you already had
	//GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1080.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// --- Camera boom ---
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = false;      // world-oriented camera (does NOT spin with character/controller)
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;

	// Collision OFF as you requested (no camera pull-in)
	CameraBoom->bDoCollisionTest = false;

	// No lag by default (locked). You can add later if desired.
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->bEnableCameraRotationLag = false;

	// Defaults
	CameraBoom->TargetArmLength = MaxArmLength;
	CameraBoom->SetRelativeRotation(FRotator(FarPitch, 0.f, 0.f));

	// --- Camera ---
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Initialize target to match defaults
	TargetArmLength = MaxArmLength;

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	// Body part meshes — all attached to the main mesh so transforms stay in sync.
	// Assign skeletal meshes and call SetLeaderPoseComponent(GetMesh()) in Blueprint BeginPlay.
	Mesh_Head  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Head"));
	Mesh_Head->SetupAttachment(GetMesh());

	Mesh_Torso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Torso"));
	Mesh_Torso->SetupAttachment(GetMesh());

	Mesh_Arms  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Arms"));
	Mesh_Arms->SetupAttachment(GetMesh());

	Mesh_Hands = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Hands"));
	Mesh_Hands->SetupAttachment(GetMesh());

	Mesh_Legs  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Legs"));
	Mesh_Legs->SetupAttachment(GetMesh());

	Mesh_Feet  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_Feet"));
	Mesh_Feet->SetupAttachment(GetMesh());
}

USkeletalMeshComponent* AFP_PlayerCharacter::GetBodyPartMesh(EBodyPart BodyPart) const
{
	switch (BodyPart)
	{
	case EBodyPart::Head:  return Mesh_Head;
	case EBodyPart::Torso: return Mesh_Torso;
	case EBodyPart::Arms:  return Mesh_Arms;
	case EBodyPart::Hands: return Mesh_Hands;
	case EBodyPart::Legs:  return Mesh_Legs;
	case EBodyPart::Feet:  return Mesh_Feet;
	default:               return nullptr;
	}
}

void AFP_PlayerCharacter::SetLeftHandIKTarget(USkeletalMeshComponent* WeaponMesh, FName SocketName, FVector JointTargetLocation)
{
	LeftHandIKWeaponMesh         = WeaponMesh;
	LeftHandIKSocket             = SocketName;
	LeftHandJointTargetLocation  = JointTargetLocation;

	// Tick the weapon mesh before the character mesh so the anim instance reads
	// a current socket position. Using TickGroup instead of AddTickPrerequisiteComponent
	// avoids a cycle with Mesh_Torso's leader-pose dependency on CharacterMesh0.
	if (WeaponMesh)
		WeaponMesh->SetTickGroup(TG_PrePhysics);
}

FVector AFP_PlayerCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	if (MontageTag.MatchesTagExact(FFP_GameplayTags::Get().Montage_Skill_Weapon))
	{
		// Search every attached skeletal mesh for one that has WeaponTipSocket.
		// The equipped weapon mesh (spawned by FFP_MeshFragment) will have it; body part meshes won't.
		TArray<USkeletalMeshComponent*> Meshes;
		GetComponents<USkeletalMeshComponent>(Meshes);
		for (USkeletalMeshComponent* SKMesh : Meshes)
		{
			if (SKMesh && SKMesh->DoesSocketExist(WeaponTipSocketName))
				return SKMesh->GetSocketLocation(WeaponTipSocketName);
		}
		// No weapon mesh with that socket found — fall back to right hand
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}

	return Super::GetCombatSocketLocation_Implementation(MontageTag);
}

void AFP_PlayerCharacter::ClearLeftHandIKTarget()
{
	if (LeftHandIKWeaponMesh.IsValid())
		LeftHandIKWeaponMesh->SetTickGroup(TG_DuringPhysics);

	LeftHandIKWeaponMesh        = nullptr;
	LeftHandIKSocket            = NAME_None;
	LeftHandJointTargetLocation = FVector::ZeroVector;
}

void AFP_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Ensure target matches current boom length (in case BP overrides values)
	if (CameraBoom)
	{
		TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, MinArmLength, MaxArmLength);
		BaseCameraBoomRelativeLocation = CameraBoom->GetRelativeLocation();
	}

	// Elevation lock reacts to the root's own TransformUpdated event rather than
	// polling in Tick() — see OnRootTransformUpdated for why.
	if (USceneComponent* Root = GetRootComponent())
	{
		Root->TransformUpdated.AddUObject(this, &AFP_PlayerCharacter::OnRootTransformUpdated);
	}
}

void AFP_PlayerCharacter::OnRootTransformUpdated(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	if (!bCameraElevationLocked || !CameraBoom || !UpdatedComponent) return;

	// Fires synchronously with whatever just moved the root (CMC crouch, the leap
	// AbilityTask's SetActorLocation, root motion, ...) so the correction can never
	// be a frame behind. Only Z is touched — X/Y stay whatever they were, so the
	// boom keeps tracking the root's horizontal movement via normal attachment.
	FVector Rel = CameraBoom->GetRelativeLocation();
	Rel.Z = LockedCameraWorldZ - UpdatedComponent->GetComponentLocation().Z;
	CameraBoom->SetRelativeLocation(Rel);
}

void AFP_PlayerCharacter::InterpCameraToDestination(FVector Destination, float InterpSpeed, float DeltaTime)
{
	if (!CameraBoom || !IsLocallyControlled()) return;

	bCameraDestinationOverride = true;
	CameraOverrideDestination  = Destination;
	CameraOverrideInterpSpeed  = FMath::Max(InterpSpeed, KINDA_SMALL_NUMBER);
}

void AFP_PlayerCharacter::ReleaseCameraInterp(float BlendOutSpeed)
{
	bCameraDestinationOverride = false;
	bCameraElevationLocked     = false;
	CameraReleaseBlendSpeed    = FMath::Max(BlendOutSpeed, KINDA_SMALL_NUMBER);
}

FVector AFP_PlayerCharacter::GetCameraFollowLocation() const
{
	return CameraBoom ? CameraBoom->GetComponentLocation() : GetActorLocation();
}

void AFP_PlayerCharacter::LockCameraElevation()
{
	if (!CameraBoom || !IsLocallyControlled()) return;

	bCameraElevationLocked = true;
	LockedCameraWorldZ     = CameraBoom->GetComponentLocation().Z;
}

void AFP_PlayerCharacter::UnlockCameraElevation(float BlendOutSpeed)
{
	bCameraElevationLocked = false;
	ReleaseCameraInterp(BlendOutSpeed);
}

void AFP_PlayerCharacter::AddCameraZoomInput(float ZoomDelta)
{
	if (!CameraBoom) return;

	// Positive wheel commonly means "scroll up" (zoom in) depending on device/settings.
	// We invert so wheel-up zooms IN (shorter arm). If yours is backwards, remove the minus.
	TargetArmLength = FMath::Clamp(
		TargetArmLength + (-ZoomDelta * ZoomStep),
		MinArmLength,
		MaxArmLength
	);
}

void AFP_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Init ability actor info for the Server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AFP_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init ability actor info for the Client
	InitAbilityActorInfo();
	
}

int32 AFP_PlayerCharacter::GetPlayerLevel()
{
	const AFP_PlayerState* FP_PlayerState = GetPlayerState<AFP_PlayerState>();
	check(FP_PlayerState);
	return FP_PlayerState->GetPlayerLevel();
}

void AFP_PlayerCharacter::AddToXP_Implementation(int32 InXP)
{
	AFP_PlayerState* FP_PlayerState = GetPlayerState<AFP_PlayerState>();
	check(FP_PlayerState);
	FP_PlayerState->AddToXP(InXP);
}

int32 AFP_PlayerCharacter::GetXP_Implementation() const
{
	const AFP_PlayerState* FP_PlayerState = GetPlayerState<AFP_PlayerState>();
	check(FP_PlayerState);
	return FP_PlayerState->GetXP();
}

int32 AFP_PlayerCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AFP_PlayerState* FP_PlayerState = GetPlayerState<AFP_PlayerState>();
	check(FP_PlayerState);
	return FP_PlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AFP_PlayerCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	const AFP_PlayerState* FP_PlayerState = GetPlayerState<AFP_PlayerState>();
	check(FP_PlayerState);
	check(FP_PlayerState->LevelUpInfo);
	return FP_PlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

void AFP_PlayerCharacter::AddToPlayerLevel_Implementation(int32 InLevel)
{
	AFP_PlayerState* FP_PlayerState = GetPlayerState<AFP_PlayerState>();
	check(FP_PlayerState);
	FP_PlayerState->AddToLevel(InLevel);
}

void AFP_PlayerCharacter::AddToAttributePoints_Implementation(int32 InPoints)
{
	AFP_PlayerState* FP_PlayerState = GetPlayerState<AFP_PlayerState>();
	check(FP_PlayerState);
	FP_PlayerState->AddToAttributePoints(InPoints);
}

int32 AFP_PlayerCharacter::GetAttributePoints_Implementation() const
{
	const AFP_PlayerState* FP_PlayerState = GetPlayerState<AFP_PlayerState>();
	check(FP_PlayerState);
	return FP_PlayerState->GetAttributePoints();
}

void AFP_PlayerCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

void AFP_PlayerCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = FollowCamera->GetComponentLocation();
		const FVector NiagaraLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}

	if (LevelUpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LevelUpSound, GetActorLocation());
	}
}

void AFP_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CameraBoom) return;

	// --- Interp speed scaling: slower when zoomed IN ---
	// t = 0 at Min (zoomed in), t = 1 at Max (zoomed out)
	const float t = FMath::GetRangePct(MinArmLength, MaxArmLength, CameraBoom->TargetArmLength);

	// When zoomed in (t near 0), scale speed down. When zoomed out (t near 1), scale up.
	// Clamp to keep it responsive.
	const float SpeedScale = FMath::Clamp(0.35f + (t * 0.65f), 0.35f, 1.0f);
	const float ArmInterpSpeed = ZoomInterpSpeed * SpeedScale;

	// Smooth arm length
	CameraBoom->TargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		TargetArmLength,
		DeltaTime,
		ArmInterpSpeed
	);

	// Smooth pitch based on zoom (optional but nice)
	const float Alpha = FMath::GetRangePct(MinArmLength, MaxArmLength, CameraBoom->TargetArmLength);
	const float DesiredPitch = FMath::Lerp(NearPitch, FarPitch, Alpha);

	FRotator R = CameraBoom->GetRelativeRotation();
	R.Pitch = FMath::FInterpTo(R.Pitch, DesiredPitch, DeltaTime, PitchInterpSpeed);
	CameraBoom->SetRelativeRotation(R);

	// --- Camera follow override (see InterpCameraToDestination) -----------------
	// Elevation lock is handled synchronously via OnRootTransformUpdated, not here —
	// skip so this block doesn't fight that correction with a stale Tick-based read.
	if (!bCameraElevationLocked)
	{
		const FVector RootLoc = GetRootComponent()->GetComponentLocation();
		const FVector Target  = bCameraDestinationOverride ? CameraOverrideDestination : (RootLoc + BaseCameraBoomRelativeLocation);
		const float   ZSpeed  = bCameraDestinationOverride ? CameraOverrideInterpSpeed : CameraReleaseBlendSpeed;
		const FVector CurrentBoomLoc = CameraBoom->GetComponentLocation();
		if (!CurrentBoomLoc.Equals(Target, 0.05f))
		{
			const FVector NewBoomLoc = FMath::VInterpTo(CurrentBoomLoc, Target, DeltaTime, ZSpeed);
			CameraBoom->SetRelativeLocation(NewBoomLoc - RootLoc);
		}
	}

	FaceMouse(DeltaTime);
}

void AFP_PlayerCharacter::InitAbilityActorInfo()
{
	AFP_PlayerState* FP_PlayerState = GetPlayerState<AFP_PlayerState>();
	check(FP_PlayerState);
	FP_PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(FP_PlayerState, this);
	Cast<UFP_AbilitySystemComponent>(FP_PlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = FP_PlayerState->GetAbilitySystemComponent();
	AttributeSet = FP_PlayerState->GetAttributeSet();
	
	if (AFP_PlayerController* FP_PlayerController = Cast<AFP_PlayerController>(GetController()))
	{
		if (AFP_HUD* FP_HUD = Cast<AFP_HUD>(FP_PlayerController->GetHUD()))
		{
			FP_HUD->InitOverlay(FP_PlayerController, FP_PlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeDefaultAttributes();

	Super::InitAbilityActorInfo();

	// Bind frozen tag so FaceMouse can be disabled while frozen.
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FFP_GameplayTags::Get().State_Frozen,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AFP_PlayerCharacter::OnFrozenTagChangedForFacing);

	AbilitySystemComponent->RegisterGameplayTagEvent(
		FFP_GameplayTags::Get().Skills_Rotation_Cancelled,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AFP_PlayerCharacter::OnRotationCancelledTagChanged);

	AbilitySystemComponent->RegisterGameplayTagEvent(
		FFP_GameplayTags::Get().Skills_Movement_DodgeRoll,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AFP_PlayerCharacter::OnDodgeRollTagChanged);
}

bool AFP_PlayerCharacter::GetMouseWorldPoint(FVector& OutWorldPoint) const
{
	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return false;

	FHitResult Hit;
	// ECC_Visibility works if your floor blocks visibility.
	// If it doesn't, create a custom trace channel for "Ground" and use that instead.
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, /*bTraceComplex=*/false, Hit))
		return false;

	if (!Hit.bBlockingHit) return false;

	OutWorldPoint = Hit.ImpactPoint;
	return true;
}

void AFP_PlayerCharacter::OnFrozenTagChangedForFacing(FGameplayTag Tag, int32 NewCount)
{
	bIsFrozen = NewCount > 0;
}

void AFP_PlayerCharacter::OnRotationCancelledTagChanged(FGameplayTag Tag, int32 NewCount)
{
	bRotationCancelled = NewCount > 0;
}

void AFP_PlayerCharacter::OnDodgeRollTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FVector Dir = GetVelocity();
		if (Dir.IsNearlyZero())
			Dir = GetCharacterMovement()->GetLastInputVector();
		Dir.Z = 0.f;
		MovementLockDir = Dir.IsNearlyZero() ? GetActorForwardVector() : Dir.GetSafeNormal();
	}
	else
	{
		MovementLockDir = FVector::ZeroVector;
	}
}

void AFP_PlayerCharacter::FaceMouse(float DeltaTime)
{
	if (bIsFrozen || bRotationCancelled) return;

	FVector MouseWorld;
	if (!GetMouseWorldPoint(MouseWorld))
		return;

	FVector ToMouse = MouseWorld - GetActorLocation();
	ToMouse.Z = 0.f;

	if (ToMouse.SizeSquared() < FMath::Square(MinAimDistance))
		return;

	const float TargetYaw = ToMouse.Rotation().Yaw;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		const FRotator Current = PC->GetControlRotation();
		const FRotator Desired(0.f, TargetYaw, 0.f);

		const FRotator Smoothed = (FacingInterpSpeed <= 0.f)
			? Desired
			: FMath::RInterpTo(Current, Desired, DeltaTime, FacingInterpSpeed);

		PC->SetControlRotation(Smoothed);
	}
}


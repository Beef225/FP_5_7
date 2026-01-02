// Copyright JG

#include "Characters/FP_PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/FP_PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

void AFP_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Ensure target matches current boom length (in case BP overrides values)
	if (CameraBoom)
	{
		TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, MinArmLength, MaxArmLength);
	}
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
}

void AFP_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init ability actor info for the Client
	InitAbilityActorInfo();
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
	
	FaceMouse(DeltaTime);
}

void AFP_PlayerCharacter::InitAbilityActorInfo()
{
	AFP_PlayerState* FP_PlayerState = GetPlayerState<AFP_PlayerState>();
	check(FP_PlayerState);
	FP_PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(FP_PlayerState, this);
	AbilitySystemComponent = FP_PlayerState->GetAbilitySystemComponent();
	AttributeSet = FP_PlayerState->GetAttributeSet();
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

void AFP_PlayerCharacter::FaceMouse(float DeltaTime)
{
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


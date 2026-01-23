// Copyright JG



#include "Player/FP_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "FP_GameplayTags.h"
#include "Input\FP_InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/FP_PlayerCharacter.h"
#include "Interaction/FP_EnemyInterface.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/SplineComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include"AbilitySystem/FP_AbilitySystemComponent.h"


AFP_PlayerController::AFP_PlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AFP_PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, bAutoRunning));
}

void AFP_PlayerController::CursorTrace()
{

	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	
	// ---------- DEBUG BLOCK ----------
	/*if (GEngine)
	{
		if (CursorHit.bBlockingHit)
		{
			AActor* HitActor = CursorHit.GetActor();
			UPrimitiveComponent* HitComp = CursorHit.GetComponent();

			const FString ActorName = HitActor ? HitActor->GetName() : TEXT("NULL");
			const FString CompName  = HitComp ? HitComp->GetName() : TEXT("NULL");

			GEngine->AddOnScreenDebugMessage(
				-1,
				0.f, // 0 = update every frame
				FColor::Green,
				FString::Printf(
					TEXT("HIT | Actor: %s | Component: %s | Distance: %.1f"),
					*ActorName,
					*CompName,
					CursorHit.Distance
				)
			);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				0.f,
				FColor::Red,
				TEXT("NO BLOCKING HIT UNDER CURSOR")
			);
		}
	}*/
	// ---------- END DEBUG BLOCK ----------
	
	AActor* HitActor = CursorHit.bBlockingHit ? CursorHit.GetActor() : nullptr;
	AActor* NewActor = (HitActor && HitActor->Implements<UFP_EnemyInterface>()) ? HitActor : nullptr;


	// If nothing changed, do nothing
	if (NewActor == ThisActor.Get())
	{
		return;
	}

	// Unhighlight previous
	if (IFP_EnemyInterface* Old = Cast<IFP_EnemyInterface>(ThisActor.Get()))
	{
		Old->UnHighlightActor();
	}

	// Highlight new
	if (IFP_EnemyInterface* New = Cast<IFP_EnemyInterface>(NewActor))
	{
		New->HighlightActor();
	}

	// Update state
	LastActor = ThisActor;
	ThisActor = NewActor;

}

void AFP_PlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
		
	const FGameplayTag MenuRoot = FGameplayTag::RequestGameplayTag(FName("InputTag.Menu"));
	if (InputTag.MatchesTag(MenuRoot))
	{
		OnUIInputTagPressed.Broadcast(InputTag);
		return; // stop here so “menu tags” don’t also try to drive abilities later
	}
	
	if (InputTag.MatchesTagExact(FFP_GameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor.IsValid();
		bAutoRunning = false;
	}
	
	
	
}

void AFP_PlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Blue, *InputTag.ToString());
	if (!InputTag.MatchesTagExact(FFP_GameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}

	if (bTargeting)
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	}
	else
	{
		if (!bMouseMoveEnabled && !bShiftKeyDown)
		{
			return;
		}
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				bAutoRunning = true;
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
	
}

void AFP_PlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Green, *InputTag.ToString());
	
	if (!InputTag.MatchesTagExact(FFP_GameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		if (!bMouseMoveEnabled && !bShiftKeyDown)
		{
			return;
		}
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
	
	
	
	
}

UFP_AbilitySystemComponent* AFP_PlayerController::GetASC()
{
	if (FP_AbilitySystemComponent==nullptr)
	{
		FP_AbilitySystemComponent = Cast<UFP_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));		
	}
	return FP_AbilitySystemComponent;
}

void AFP_PlayerController::AutoRun()
{
	if (!bMouseMoveEnabled) return;
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AFP_PlayerController::SetMouseMoveEnabled(bool bEnabled)
{
	if (bMouseMoveEnabled == bEnabled)
	{
		return;
	}
	bMouseMoveEnabled = bEnabled;
}

void AFP_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(FPContext);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(FPContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);


}

void AFP_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UFP_InputComponent* FP_InputComponent = CastChecked<UFP_InputComponent>(InputComponent);
	FP_InputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFP_PlayerController::Move);
	FP_InputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AFP_PlayerController::Zoom);
	FP_InputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AFP_PlayerController::ShiftPressed);
	FP_InputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AFP_PlayerController::ShiftReleased);
	FP_InputComponent->BindAbilityActions(InputConfig,this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AFP_PlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (bMouseMoveEnabled)
	{
		return; // mouse-only mode
	}

	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	const FVector ForwardDirection = FVector::ForwardVector; // world +X
	const FVector RightDirection   = FVector::RightVector;   // world +Y

	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,   InputAxisVector.X);
	}
	
}

void AFP_PlayerController::Zoom(const FInputActionValue& InputActionValue)
{
	const float ZoomValue = InputActionValue.Get<float>();

	if (FMath::IsNearlyZero(ZoomValue))
	{
		return;
	}

	if (AFP_PlayerCharacter* PC = GetPawn<AFP_PlayerCharacter>())
	{
		PC->AddCameraZoomInput(ZoomValue);
	}
}



// Copyright JG



#include "Player/FP_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "Input\FP_InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/FP_PlayerCharacter.h"
#include "Interaction/FP_EnemyInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include"AbilitySystem/FP_AbilitySystemComponent.h"


AFP_PlayerController::AFP_PlayerController()
{
	bReplicates = true;
}

void AFP_PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AFP_PlayerController::CursorTrace()
{
	FHitResult CursorHit;
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
	
	if (!CursorHit.bBlockingHit) return;
	
	LastActor=ThisActor;
	ThisActor = CursorHit.GetActor();
	
	/**
	 * Line trace from cursor. There are several scenarios:
	 *  A. LastActor is null && ThisActor is null
	 *		- Do nothing
	 *	B. LastActor is null && ThisActor is valid
	 *		- Highlight ThisActor
	 *	C. LastActor is valid && ThisActor is null
	 *		- UnHighlight LastActor
	 *	D. Both actors are valid, but LastActor != ThisActor
	 *		- UnHighlight LastActor, and Highlight ThisActor
	 *	E. Both actors are valid, and are the same actor
	 *		- Do nothing
	 */
	
	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// Case B
			ThisActor->HighlightActor();
		}
		else
		{
			// Case A - both are null, do nothing
		}
	}
	else // LastActor is valid
	{
		if (ThisActor == nullptr)
		{
			// Case C
			LastActor->UnHighlightActor();
		}
		else // both actors are valid
		{
			if (LastActor != ThisActor)
			{
				// Case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				// Case E - do nothing
			}
		}
	}
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
	
	
	
}

void AFP_PlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Blue, *InputTag.ToString());
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AFP_PlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Green, *InputTag.ToString());
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagHeld(InputTag);
}

UFP_AbilitySystemComponent* AFP_PlayerController::GetASC()
{
	if (FP_AbilitySystemComponent==nullptr)
	{
		FP_AbilitySystemComponent = Cast<UFP_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));		
	}
	return FP_AbilitySystemComponent;
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
	FP_InputComponent->BindAbilityActions(InputConfig,this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AFP_PlayerController::Move(const FInputActionValue& InputActionValue)
{
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



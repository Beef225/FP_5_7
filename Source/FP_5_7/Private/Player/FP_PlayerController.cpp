// Copyright JG



#include "Player/FP_PlayerController.h"
#include "Actor/Items/FP_ItemActor.h"
#include "EnhancedInputComponent.h"
#include "FP_GameplayTags.h"
#include "Input\FP_InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/FP_PlayerCharacter.h"
#include "Interaction/FP_EnemyInterface.h"
#include "Interaction/FP_HighlightInterface.h"
#include "Interaction/FP_InteractableInterface.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/SplineComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "UI/Widget/Inventory/FP_InventoryBase.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "SaveSystem/FP_SaveGameSubsystem.h"
#include "Player/FP_PlayerState.h"


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
	AActor* NewActor = (IsValid(HitActor) && HitActor->Implements<UFP_HighlightInterface>()) ? HitActor : nullptr;

	// If nothing changed, do nothing
	if (NewActor == ThisActor)
	{
		return;
	}

	UnHighlightActor(ThisActor);
	HighlightActor(NewActor);

	LastActor = ThisActor;
	ThisActor = NewActor;

}

void AFP_PlayerController::HighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UFP_HighlightInterface>())
	{
		IFP_HighlightInterface::Execute_HighlightActor(InActor);
	}
}

void AFP_PlayerController::UnHighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UFP_HighlightInterface>())
	{
		IFP_HighlightInterface::Execute_UnHighlightActor(InActor);
	}
}

void AFP_PlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
		
	const FGameplayTag MenuRoot = FGameplayTag::RequestGameplayTag(FName("InputTag.Menu"));
	if (InputTag.MatchesTag(MenuRoot))
	{
		bPendingLevelTransition = false;
		OnUIInputTagPressed.Broadcast(InputTag);
		return; // stop here so "menu tags" don’t also try to drive abilities later
	}

	if (InputTag.MatchesTagExact(FFP_GameplayTags::Get().InputTag_LMB))
	{
		// Drop hover item if clicking outside the inventory widget bounds.
		// AbilityInputTagPressed fires after Slate processes the frame's input events, so if the
		// click landed on the grid or an equipment slot, Slate already cleared the hover item —
		// HasHoverItem() will be false and we skip this. If the hover item is still set, nothing
		// in the UI consumed the click (world click), so we drop.
		UFP_InventoryComponent* IC = UFP_AbilitySystemLibrary::GetInventoryComponent(this);
		if (IsValid(IC) && IsValid(IC->GetInventoryMenu()) && IC->GetInventoryMenu()->HasHoverItem())
		{
			const FVector2D AbsCursorPos = FSlateApplication::Get().GetCursorPos();
			if (!IC->GetInventoryMenu()->GetCachedGeometry().IsUnderLocation(AbsCursorPos))
			{
				IC->GetInventoryMenu()->DropHoverItem();
				bAutoRunning = false;
				bPendingLevelTransition = false;
				PendingPickupItem = nullptr;
				return;
			}
		}

		bPendingLevelTransition = false;
		PendingPickupItem = nullptr;
		if (IsValid(ThisActor))
		{
			TargetingStatus = ThisActor->Implements<UFP_EnemyInterface>()
				? EFP_TargetingStatus::TargetingEnemy
				: EFP_TargetingStatus::TargetingNonEnemy;
		}
		else
		{
			TargetingStatus = EFP_TargetingStatus::NotTargeting;
		}
		bAutoRunning = false;
	}
	else
	{
		// Any non-LMB ability (skills, etc.) cancels a pending transition
		bPendingLevelTransition = false;
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

	if (TargetingStatus == EFP_TargetingStatus::TargetingEnemy)
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	}
	else
	{
		// Allow movement when targeting a non-enemy interactable even if mouse-move is off
		const bool bTargetingInteractable = TargetingStatus == EFP_TargetingStatus::TargetingNonEnemy;
		if (!bMouseMoveEnabled && !bShiftKeyDown && !bTargetingInteractable)
		{
			return;
		}

		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// Let the clicked actor redirect the destination (e.g. door's MoveToComponent)
			if (IsValid(ThisActor) && ThisActor->Implements<UFP_HighlightInterface>())
			{
				IFP_HighlightInterface::Execute_SetMoveToLocation(ThisActor, CachedDestination);
				if (bTargetingInteractable)
				{
					bPendingLevelTransition = true;
				}
			}

			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}
				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
					bAutoRunning = true;
				}
			}
		}
		FollowTime = 0.f;
		TargetingStatus = EFP_TargetingStatus::NotTargeting;
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

	if (TargetingStatus == EFP_TargetingStatus::TargetingEnemy || bShiftKeyDown)
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
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		if (PendingPickupItem.IsValid() && IsInPickupRange(PendingPickupItem.Get()))
		{
			bAutoRunning = false;
			PendingPickupItem->OnPickupRequested();
			PendingPickupItem = nullptr;
			return;
		}

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

bool AFP_PlayerController::IsInPickupRange(const AFP_ItemActor* ItemActor) const
{
	const APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !IsValid(ItemActor)) return false;
	return FVector::DistSquared(ControlledPawn->GetActorLocation(), ItemActor->GetActorLocation()) <= PickupRange * PickupRange;
}

void AFP_PlayerController::AutoRunToItem(AFP_ItemActor* ItemActor)
{
	if (!IsValid(ItemActor)) return;
	const APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), ItemActor->GetActorLocation()))
	{
		Spline->ClearSplinePoints();
		for (const FVector& PointLoc : NavPath->PathPoints)
		{
			Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
		}
		if (NavPath->PathPoints.Num() > 0)
		{
			CachedDestination = NavPath->PathPoints.Last();
			bAutoRunning = true;
			PendingPickupItem = ItemActor;
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

	if (IsLocalController() && GameplayHUDClass)
	{
		GameplayHUDWidget = CreateWidget<UUserWidget>(this, GameplayHUDClass);
		GameplayHUDWidget->AddToViewport();
	}
}

void AFP_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UFP_SaveGameSubsystem* SaveSys = UFP_SaveGameSubsystem::Get(this))
	{
		if (const FFP_CharacterSaveRecord* Record = SaveSys->GetPendingCharacterRecord())
		{
			if (AFP_PlayerState* PS = GetPlayerState<AFP_PlayerState>())
			{
				PS->SetXP(Record->ExperiencePoints);
				PS->SetLevel(Record->CharacterLevel);
				PS->LoadAllocatedPoints(
					Record->UnspentAttributePoints,
					Record->MightPoints,
					Record->ResonancePoints,
					Record->AgilityPoints,
					Record->FortitudePoints
				);
			}
		}
		SaveSys->ClearPendingCharacter();
	}
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

	bAutoRunning = false;
	bPendingLevelTransition = false;

	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	const FVector ForwardDirection = FVector::ForwardVector; // world +X
	const FVector RightDirection   = FVector::RightVector;   // world +Y

	if (APawn* ControlledPawn = GetPawn())
	{
		if (AFP_PlayerCharacter* PC = Cast<AFP_PlayerCharacter>(ControlledPawn))
		{
			const FVector LockDir = PC->GetMovementLockDir();
			if (!LockDir.IsZero())
			{
				const FVector WorldInput = ForwardDirection * InputAxisVector.Y + RightDirection * InputAxisVector.X;
				if (!WorldInput.IsNearlyZero())
				{
					const float Dot = FVector::DotProduct(WorldInput.GetSafeNormal(), LockDir);
					if (Dot < 0.7071f)
						return;
				}
			}
		}

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



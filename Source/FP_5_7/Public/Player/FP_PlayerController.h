
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "FP_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UFP_InputConfig;
class UFP_AbilitySystemComponent;
class USplineComponent;
class UUserWidget;
class AFP_ItemActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIInputTagSignature, FGameplayTag, InputTag);

UENUM()
enum class EFP_TargetingStatus : uint8
{
	TargetingEnemy,
	TargetingNonEnemy,
	NotTargeting
};
/**
 * 
 */
UCLASS()
class FP_5_7_API AFP_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AFP_PlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	
	UPROPERTY(BlueprintAssignable, Category="Input|UI")
	FOnUIInputTagSignature OnUIInputTagPressed;

	UPROPERTY(BlueprintAssignable, Category="Input|UI")
	FOnUIInputTagSignature OnUIInputTagReleased;

	UFUNCTION(BlueprintCallable, Category="Input")
	void SetMouseMoveEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category="Input")
	bool IsMouseMoveEnabled() const { return bMouseMoveEnabled; }

	UFUNCTION(BlueprintCallable, Category="Game|Pause")
	void PauseGame();

	UFUNCTION(BlueprintCallable, Category="Game|Pause")
	void UnpauseGame();

	bool IsPendingInteractableArrival() const { return bPendingInteractableArrival; }

	/** Call once the target actor's own trigger sphere detects arrival — clears the flag and stops the walk-to-it auto-run. */
	void ConsumePendingInteractableArrival() { bPendingInteractableArrival = false; bAutoRunning = false; }

	/**
	 * Triggers the same interaction a world-click on TargetActor would: walks the
	 * player there and then interacts, or interacts immediately if already in range.
	 * TargetActor must implement both IFP_HighlightInterface (for SetMoveToLocation)
	 * and IFP_InteractableInterface. Intended for UI affordances like
	 * UFP_InteractionPromptWidget's button — not used by the world-click flow itself,
	 * which has its own inline logic in AbilityInputTagReleased.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void RequestInteraction(AActor* TargetActor);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsInPickupRange(const AFP_ItemActor* ItemActor) const;

	void AutoRunToItem(AFP_ItemActor* ItemActor);

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float PickupRange = 400.f;
	

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	
private:

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> FPContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ZoomAction;

	void Zoom(const FInputActionValue& InputActionValue);
	
	void CursorTrace();
	static void HighlightActor(AActor* InActor);
	static void UnHighlightActor(AActor* InActor);

	TObjectPtr<AActor> LastActor;
	TObjectPtr<AActor> ThisActor;
	FHitResult CursorHit;
	
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UFP_InputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UFP_AbilitySystemComponent> FP_AbilitySystemComponent;
	
	UFP_AbilitySystemComponent* GetASC();
	
	UPROPERTY(EditDefaultsOnly, Category="HUD")
	TSubclassOf<UUserWidget> GameplayHUDClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> GameplayHUDWidget;

	UPROPERTY(EditAnywhere, Category="Input")
	bool bMouseMoveEnabled = false;
	
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	EFP_TargetingStatus TargetingStatus = EFP_TargetingStatus::NotTargeting;

	/**
	 * Set when the player short-clicks a highlighted non-enemy interactable (door,
	 * loot container, ...) and is walking to it. The target actor's own trigger
	 * sphere consumes this on overlap to fire its arrival behavior. Cleared by any
	 * other input.
	 */
	bool bPendingInteractableArrival = false;

	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	bool bShiftKeyDown = false;


	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	TWeakObjectPtr<AFP_ItemActor> PendingPickupItem;

	void AutoRun();
	
};


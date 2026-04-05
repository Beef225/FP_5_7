
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

	bool IsPendingLevelTransition() const { return bPendingLevelTransition; }
	void ConsumePendingLevelTransition() { bPendingLevelTransition = false; }
	

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

	/** Set when the player intentionally clicks a level transition actor. Cleared by any other input. */
	bool bPendingLevelTransition = false;

	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	bool bShiftKeyDown = false;


	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
	
	void AutoRun();
	
};


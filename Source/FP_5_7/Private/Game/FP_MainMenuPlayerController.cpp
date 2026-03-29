// Copyright JG

#include "Game/FP_MainMenuPlayerController.h"


void AFP_MainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Show cursor and route input to UI only — no game input in the main menu
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
}

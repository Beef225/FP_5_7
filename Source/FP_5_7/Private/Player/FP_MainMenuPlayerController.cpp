// Copyright JG

#include "Player/FP_MainMenuPlayerController.h"
#include "SaveSystem/FP_SaveGameSubsystem.h"
#include "Locations/FP_LocationRegistry.h"


void AFP_MainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Show cursor and route input to UI only — no game input in the main menu
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());

	if (LocationRegistry)
	{
		if (UFP_SaveGameSubsystem* SaveSys = UFP_SaveGameSubsystem::Get(this))
		{
			SaveSys->SetLocationRegistry(LocationRegistry);
		}
	}
}

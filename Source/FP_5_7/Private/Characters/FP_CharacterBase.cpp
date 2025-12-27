// Copyright JG
#include "Characters/FP_CharacterBase.h"

// Sets default values
AFP_CharacterBase::AFP_CharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}


void AFP_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}



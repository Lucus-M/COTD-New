// Copyright Epic Games, Inc. All Rights Reserved.

#include "COTDGameMode.h"
#include "COTDCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACOTDGameMode::ACOTDGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

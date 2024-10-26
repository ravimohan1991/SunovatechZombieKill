// Copyright Epic Games, Inc. All Rights Reserved.

#include "SunovatechZombieKillGameMode.h"
#include "SunovatechZombieKillPlayerController.h"

ASunovatechZombieKillGameMode::ASunovatechZombieKillGameMode()
{
	PlayerControllerClass = ASunovatechZombieKillPlayerController::StaticClass();
}

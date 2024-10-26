// Copyright Epic Games, Inc. All Rights Reserved.

#include "Vehicles/SunovatechZombieKillWheelRear.h"
#include "UObject/ConstructorHelpers.h"

USunovatechZombieKillWheelRear::USunovatechZombieKillWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}
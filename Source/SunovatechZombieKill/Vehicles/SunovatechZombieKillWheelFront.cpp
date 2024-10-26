// Copyright Epic Games, Inc. All Rights Reserved.

#include "SunovatechZombieKillWheelFront.h"
#include "UObject/ConstructorHelpers.h"

USunovatechZombieKillWheelFront::USunovatechZombieKillWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}
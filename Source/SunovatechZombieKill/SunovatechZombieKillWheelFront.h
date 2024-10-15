/**
 * @file SunovatechZombieKillWheelFront.h
 * @author Ravi Mohan
 * @brief This file contains USunovatechZombieKillWheelFront. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 17, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */

#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "SunovatechZombieKillWheelFront.generated.h"

/**
 *  Base front wheel definition.
 */
UCLASS()
class USunovatechZombieKillWheelFront : public UChaosVehicleWheel
{
	GENERATED_BODY()

public:

	/**
	 * @brief Constructor for setting axle type and maximum steering angle
	 *
	 */
	USunovatechZombieKillWheelFront();
};

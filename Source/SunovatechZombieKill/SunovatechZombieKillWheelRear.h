/**
 * @file SunovatechZombieKilWheelRearl.h
 * @author Ravi Mohan
 * @brief This file contains USunovatechZombieKillWheelRear. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 17, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */
#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "SunovatechZombieKillWheelRear.generated.h"

/**
 *  Base rear wheel definition.
 */
UCLASS()
class USunovatechZombieKillWheelRear : public UChaosVehicleWheel
{
	GENERATED_BODY()

public:

	/**
	 * @brief A constructor to set up axle type and hanbreak / Engine affect boolean.
	 */
	USunovatechZombieKillWheelRear();
};

/**
 * @file SunovatechZombieKillOffroadWheelRear.h
 * @author Ravi Mohan
 * @brief This file contains USunovatechZombieKillOffroadWheelRear class. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 17, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */

#pragma once

#include "CoreMinimal.h"
#include "SunovatechZombieKillWheelRear.h"
#include "SunovatechZombieKillOffroadWheelRear.generated.h"

/**
 *  Rear wheel definition for Offroad Car.
 */
UCLASS()
class SUNOVATECHZOMBIEKILL_API USunovatechZombieKillOffroadWheelRear : public USunovatechZombieKillWheelRear
{
	GENERATED_BODY()
	
public:

	/**
	 * @brief Constructor for setting wheel dimension, physics (friction, wheel load ratio ?, suspension, and break torque(s)), and vehicle engine
	 *
	 */
	USunovatechZombieKillOffroadWheelRear();
};

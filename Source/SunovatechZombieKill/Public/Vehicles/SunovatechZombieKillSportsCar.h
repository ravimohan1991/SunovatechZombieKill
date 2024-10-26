/**
 * @file SunovatechZombieKillSportsCar.h
 * @author Ravi Mohan
 * @brief This file contains player controller class for vehicle pawn. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 17, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */

#pragma once

#include "CoreMinimal.h"
#include "SunovatechZombieKill/Public/SunovatechZombieKillPawn.h"
#include "SunovatechZombieKillSportsCar.generated.h"

/**
 * @brief Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class SUNOVATECHZOMBIEKILL_API ASunovatechZombieKillSportsCar : public ASunovatechZombieKillPawn
{
	GENERATED_BODY()
	
public:

	/**
	 * @brief Sets up various car components
	 *
	 * Components set are like so
	 *  1. Chassis mesh
	 *  2. All four tire mesh compoenets
	 *  3. Cameras (front and rear)
	 *  4. Chaos vehicle movement parameters
	 *  5. The parameters are chassis, wheels, engine, differential, and steering
	 */
	ASunovatechZombieKillSportsCar();
};

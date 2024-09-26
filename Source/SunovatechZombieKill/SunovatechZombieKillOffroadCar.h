/**
 * @file SunovatechZombieKillOffroadCar.h
 * @author Ravi Mohan
 * @brief This file contains ASunovatechZombieKillOffroadCar class. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 17, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */

#pragma once

#include "CoreMinimal.h"
#include "SunovatechZombieKillPawn.h"
#include "SunovatechZombieKillOffroadCar.generated.h"

/**
 *  Offroad car wheeled vehicle implementation
 */
UCLASS(abstract)
class SUNOVATECHZOMBIEKILL_API ASunovatechZombieKillOffroadCar : public ASunovatechZombieKillPawn
{
	GENERATED_BODY()
	
	/** Chassis static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Chassis;

	/** FL Tire static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireFrontLeft;

	/** FR Tire static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireFrontRight;

	/** RL Tire static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireRearLeft;

	/** RR Tire static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireRearRight;

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
	 *
	 * @note We are not using this offread car
	 */
	ASunovatechZombieKillOffroadCar();
};

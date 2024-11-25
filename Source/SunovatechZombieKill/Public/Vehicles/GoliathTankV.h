// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SunovatechZombieKillPawn.h"
#include "GoliathTankV.generated.h"

/**
 * @brief Pawn class of the Tank (named Goliath) taken from Vigilante content
 */
UCLASS()
class SUNOVATECHZOMBIEKILL_API AGoliathTankV : public ASunovatechZombieKillPawn
{
	GENERATED_BODY()

protected:

	/**
	 * @brief For tracking the location of the tip of tank's gun
	 */
	UPROPERTY()
	class USceneComponent* TankGunMuzzleLocation;

	/**
	 * @brief Reference to the animation instance being used by Goliath
	 */
	//UPROPERTY(EditDefaultsOnly, Category = "Assets")
	//TSubclassOf<class VechicleAnimInstance> AnimationBP;

	/**
	 * @brief Caching the angle of tank's gun
	 */
	float GunAngle;

public:
	AGoliathTankV();
};

/**
 * @file GoliathTankV.h
 * @author Ravi Mohan
 * @brief This file contains vehicle pawn class AGoliathTankV. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date November 25, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */
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
	UPROPERTY(BlueprintReadOnly)
	class USceneComponent* TankGunMuzzleLocation;

	/**
	 * @brief Reference to the animation instance being used by Goliath
	 * 
	 * @note The modifications within the animation blueprint are done from blueprint side atm. 
	 * The pawn specific variables, for instance, tank speed in animation blueprint can only be done blueprint side only.
	 * 
	 * @todo Ponder over nativization
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	class UVehicleAnimationInstance* AnimationBP;

	/**
	 * @brief Caching the angle of tank's gun
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	float GunAngle;

public:
	/**
	 * @brief Constructor to do the following
	 * 1. Create and attach a scene component to tank's mesh
	 * 2. 
	 */
	AGoliathTankV();

	/**
	 * @brief Allows a Pawn to set up custom input bindings. Called upon possession by a PlayerController, using the InputComponent created by CreatePlayerInputComponent().
	 *
	 * @param UInputComponent								The transient component that enables an Actor to bind various forms of input events to delegate functions.
	 *
	 * @note We are using UE5's new Enhanced Input (https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine)
	 * @see ASunovatechZombieKillPlayerController::SetupInputComponent()
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/**
	 * @brief A native event for when play begins for this actor
	 *
	 * @note Using this for setting Health.
	 * @see ASunovatechZombieKillPawn::GetMaxHealth()
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Function called every frame on this Actor. For custom logic, Epic does some angular damping activity.
	 *
	 * @param Delta							Game time elapsed during last frame, modified by the time dilation
	 *
	 * @note Seems like PrimaryActorTick.bCanEverTick = true is no longer needed in 5.4 (against what is documented in AActor class)? Also,
	 * for our purposes, we can do without angular damping?
	 */
	virtual void Tick(float Delta) override;

	/**
	 * @brief Computes the tank's turret angle
	 * 
	 * @todo Think about the category
	 */
	UFUNCTION(Blueprintpure, Category = "Animation")
	float GetTurretAngle(float InterpolateSpeed);

protected:

	/** Handles steering trigger input */
	void SteeringTrigger(const FInputActionValue& Value);

	/**
	 * Handles steering complete input
	 */
	void SteeringComplete(const FInputActionValue& Value);
};

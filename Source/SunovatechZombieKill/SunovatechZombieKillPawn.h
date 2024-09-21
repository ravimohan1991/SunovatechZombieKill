/**
 * @file SunovatechZombieKillPawn.h
 * @author Ravi Mohan
 * @brief This file contains vehicle pawn class. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 17, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */
#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "SunovatechZombieKillPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
class ASunovatechZombieKillStProjectile;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateVehicle, Log, All);

/**
 *  Vehicle Pawn class
 *  Handles common functionality for all vehicle types,
 *  including input handling and camera management.
 *  
 *  Specific vehicle configurations are handled in subclasses.
 */
UCLASS(abstract)
class ASunovatechZombieKillPawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	/* To detect overlap events. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Zombie Interaction")
	class USphereComponent* OverlapComp;

	/** Spring Arm for the front camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FrontSpringArm;

	/** Front Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FrontCamera;

	/** Spring Arm for the back camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	/** Back Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;

	/** Cast pointer to the Chaos Vehicle movement component */
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

protected:

	/** Shoot action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ShootAction;

	/** Steering Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;

	/** Throttle Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;

	/** Brake Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;

	/** Handbrake Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HandbrakeAction;

	/** Look Around Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;

	/** Toggle Camera Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleCameraAction;

	/** Reset Vehicle Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ResetVehicleAction;

	/** Projectile class to spawn on shooting */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ASunovatechZombieKillStProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	USoundBase* FireSound;

	/** Player health */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float Health;

	/* Number of zombies melee attacking */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Interaction")
	int32 ZombiesAttacking;

	/** Keeps track of which camera is active */
	bool bFrontCameraActive = false;

	/* Handle to manage the timer */
	FTimerHandle HurtTimerHandle;

public:
	ASunovatechZombieKillPawn();

	/**
	 * Called when this vechicle pawn begins overlaps different actor
	 *
	 * Useful to detect the zombie touch (melee) events
	 *
	 */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Called when this vechicle pawn ends overlaps different actor
	 *
	 * Useful to detect the zombie touch (melee) events
	 *
	 */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// Begin Pawn interface

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// End Pawn interface

	// Begin Actor interface

	virtual void Tick(float Delta) override;

	// End Actor interface

	/************************************************************************/
	/* Health (taken from ASunovatechZombieKillZoCharacter)                 */
	/* Probabbly could make a native health component                       */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	bool IsAlive() const;

	/**
	 * @brief A very experimental routine with the assumption that damage inflictor are zombies only.
	 * Typically you would want APawn::TakeDamage or something like that. This routine will be used
	 * for zombie's melee attack
	 * 
	 * @note Different from usual practice
	 */
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float PlayerHurt(float DamageAmount = 1.0f);

protected:

	/** Handles steering input */
	void Steering(const FInputActionValue& Value);

	/** Handles throttle input */
	void Throttle(const FInputActionValue& Value);

	/** Handles brake input */
	void Brake(const FInputActionValue& Value);

	/** Handles brake start/stop inputs */
	void StartBrake(const FInputActionValue& Value);
	void StopBrake(const FInputActionValue& Value);

	/** Handles handbrake start/stop inputs */
	void StartHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	/** Handles look around input */
	void LookAround(const FInputActionValue& Value);

	/** Handles toggle camera input */
	void ToggleCamera(const FInputActionValue& Value);

	/** Handles reset vehicle input */
	void ResetVehicle(const FInputActionValue& Value);

	/** Called when the brake lights are turned on or off */
	UFUNCTION(BlueprintImplementableEvent, Category="Vehicle")
	void BrakeLights(bool bBraking);

	/** 
	 * @brief Fires a projectile.
	 * 
	 * @note No network play compatible logic
	 */
	void Fire();

public:
	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

public:
	/** Returns the front spring arm subobject */
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
	/** Returns the front camera subobject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }
	/** Returns the back spring arm subobject */
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	/** Returns the back camera subobject */
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	/** Returns the cast Chaos Vehicle Movement subobject */
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
};

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

/**
 * @brief Declaring our own logging category
 *
 * @param CategoryName								category to declare
 * @param DefaultVerbosity							default run time verbosity
 * @param CompileTimeVerbosity						maximum verbosity to compile into the code
 */
DECLARE_LOG_CATEGORY_EXTERN(LogSunovatechZombieKill, Log, All);

/**
 *  @brief Vehicle Pawn class
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

protected:

	/** Cast pointer to the Chaos Vehicle movement component */
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

	/************************************************************************/
	/* Input                                                               */
	/************************************************************************/

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

	/** Zooming action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AimZoomAction;

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

	/** Amount of damage to be given cached */
	//UPROPERTY(BlueprintReadOnly, Category = "Zombie Interaction")
	//float IndividualZombieDamage;

	/** Caching the original FOV */
	float OriginalFOV;

	/** Intermediate FOV value during lerping */
	float LerpingFOV;

	/** Should we zoom? */
	bool bZoom;

	/** Keeps track of which camera is active */
	bool bFrontCameraActive = false;

	/* Handle to manage the timer */
	FTimerHandle HurtTimerHandle;

public:

	/**
	 * @brief A constructor
	 *
	 * Construction to set up the following:
	 *
	 * 1. Camera components on the vehicle (front and back)
	 * 2. Mesh with simulated physics on
	 * 3. Chaos vehiclular movement
	 * 4. Initialization of Health and ZombiesAttacking
	 */
	ASunovatechZombieKillPawn();

	/**
	 * @brief Called when this vechicle pawn begins overlapping different actor.
	 *
	 * To be bound to the overlap delegate (OnComponentBeginOverlap) declared in PrimitiveComponent.h . Useful to detect the zombie touch (melee) events, when zombie effectively
	 * (depending on the radius of USphereComponent) starts touching vehicle pawn
	 *
	 * @param OverlappedComponent							The overlap component of the pawn vehicle
	 * @param OtherActor									The actor interacting with pawn vehicle leading to generation of the event beginoverlap
	 * @param OtherComp										The overlap component (?) of the interacting actor
	 * @param OtherBodyIndex								Extra data about item that was hit (hit primitive specific).
	 * @param bFromSweep									If bFromSweep is true then SweepResult (type FHitResult) is completely valid just like
	 * 														a regular sweep result. If bFromSweep is false only FHitResult::Component,
	 * 														FHitResult::Actor, FHitResult::Item are valid as this is really just an FOverlapResult
	 * @param SweepResult									Information for both sweep and overlap queries. Different parts are valid depending on bFromSweep.
	 * 														Essentially, information about one hit of a trace, such as point of impact and surface normal at that point
	 * 														(declared in Hitresult.h).
	 * 	@see OverlapInfo.h, Hitresult.h
	 *
	 */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * @brief Called when this vechicle pawn ends overlapping different actor
	 *
	 * To be bound to overlap delegate (OnComponentEndOverlap) declared in PrimitiveComponent.h . Useful to detect when to end zombie melee events, when zombie effectively
	 * (depending on the radius of USphereComponent) ends touching vehicle pawn
	 *
	 * @param OverlappedComponent							The overlap component of the pawn vehicle
	 * @param OtherActor									The actor interacting with pawn vehicle leading to generation of the event endoverlap
	 * @param OtherComp										The overlap component (?) of the interacting actor
	 * @param OtherBodyIndex								Extra data about item that was hit (hit primitive specific).
	 *
	 * @see OverlapInfo.h
	 */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// Begin Pawn interface

	/**
	 * @brief Allows a Pawn to set up custom input bindings. Called upon possession by a PlayerController, using the InputComponent created by CreatePlayerInputComponent().
	 *
	 * @param UInputComponent								The transient component that enables an Actor to bind various forms of input events to delegate functions.
	 *
	 * @note We are using UE5's new Enhanced Input (https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine)
	 * @see ASunovatechZombieKillPlayerController::SetupInputComponent()
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// End Pawn interface

	// Begin Actor interface

	/**
	 * @brief Function called every frame on this Actor. For custom logic, Epic does some angular damping activity.
	 *
	 * @param Delta							Game time elapsed during last frame, modified by the time dilation
	 *
	 * @note Seems like PrimaryActorTick.bCanEverTick = true is no longer needed in 5.4 (against what is documented in AActor class)? Also,
	 * for our purposes, we can do without angular damping?
	 */
	virtual void Tick(float Delta) override;

	// End Actor interface

	/************************************************************************/
	/* Health (taken from ASunovatechZombieKillZoCharacter)                 */
	/* Probabbly could make a native health component                       */
	/************************************************************************/

	/**
	 * @brief Getter for this pawn's initialized health
	 *
	 * @return Health
	 *
	 * @note For some reason, seems like setting Health in constructor and blueprint is not enough.
	 * Had to set the value in pawn's BeginPlay() routine.
	 *
	 * @see ASunovatechZombieKillPawn::BeginPlay()
	 */
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHealth() const;

	/**
	 * @brief Gets the current health of this pawn
	 *
	 * @return Health
	 *
	 * @note Can be called from blueprints
	 */
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHealth() const;

	/**
	 * @brief Function for querying about the player's state of being, in the game. Dead or alive.
	 *
	 * @return True if Health > 0, else, False
	 */
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	bool IsAlive() const;

	/**
	 * @brief Gets the current active camera being used for the view purpose
	 * 
	 * @return Active camera
	 */
	UFUNCTION(BlueprintCAllable, Category = "Accessories")
	UCameraComponent* GetActiveCamera() const;

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

	/** Zooming in of the aim */
	void ZoomIn(const FInputActionValue& Value);

	/** Zoom out */
	void ZoomOut(const FInputActionValue& Value);

	/** 
	 * @brief Fires a projectile.
	 * 
	 * @note No network play compatible logic
	 */
	virtual void Fire();

	/**
	 * @brief A native event for when play begins for this actor
	 *
	 * @note Using this for setting Health.
	 * @see ASunovatechZombieKillPawn::GetMaxHealth()
	 */
	virtual void BeginPlay() override;
	
	/**
	 * @brief Handles Endgame things to be done which we will do in blueprints
	 *
	 * @note Typically this type of routine belongs to Gamemode. Also seems like not working in blueprint
	 * @todo Fix the blueprint functionality
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay")
	void EndGame();

public:

	/**
	 * @brief Gun muzzle's offset from the pawn location. Typically slightly above the roof of the vehicle
	 *
	 * @todo Myabe EditDefaultsOnly should be specified instead?
	 * @note In my training, I was advised against such public declaration. UE does simillar declarations in some cases though. Needs thinking.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	FVector MuzzleOffset;

	/** Offset from center of screen for reticle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	FVector2D OffsetFromCenter;

public:

	/** 
	 * @brief Getter for Reticle offset from center
	 */
	FVector2D GetReticleOffsetFromCenter() const { return OffsetFromCenter; }

	/**
	 * @brief Returns the front spring arm subobject
	 *
	 * @return FrontSpringArm
	 */
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }

	/**
	 * @brief Returns the front camera subobject
	 *
	 * @return FrontCamera
	 */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }

	/**
	 * @brief Returns the back spring arm subobject
	 *
	 * @return BackSpringArm
	 */
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }

	/**
	 * @brief Returns the back camera subobject
	 *
	 * @return BackCamera
	 */
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }

	/**
	 * @brief Returns the cast Chaos Vehicle Movement subobject
	 *
	 * @return ChaosVehicleMovement
	 */
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
};

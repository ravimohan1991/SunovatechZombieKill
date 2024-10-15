/**
 * @file SunovatechZombieKillPlayerController.h
 * @author Ravi Mohan
 * @brief This file contains player controller class for vehicle pawn. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 17, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SunovatechZombieKillPlayerController.generated.h"

class UInputMappingContext;
class ASunovatechZombieKillPawn;
class USunovatechZombieKillUI;

/**
 * @brief Vehicle Player Controller class.
 * Handles input mapping and user interface
 */
UCLASS(abstract)
class SUNOVATECHZOMBIEKILL_API ASunovatechZombieKillPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	/** Pointer to the controlled vehicle pawn */
	TObjectPtr<ASunovatechZombieKillPawn> VehiclePawn;

	/** Type of the UI to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<USunovatechZombieKillUI> VehicleUIClass;

	/** Pointer to the UI widget */
	TObjectPtr<USunovatechZombieKillUI> VehicleUI;

	// Begin Actor interface
protected:

	/**
	 *  @brief A native event for when play begins for this actor
	 *
	 * A vehicle widget UI is created for display of speed and gear on screen (imitating HUD).
	 *
	 * @note We have written the proper HUD, ASunovatechZombieKillHUD class, for the purpose. Later we may move the UI code to HUD, the
	 * way it should be done.
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Setup an input mode
	 *
	 * @note Using UEnhancedInputLocalPlayerSubsystem
	 */
	virtual void SetupInputComponent() override;

public:

	/**
	 * @brief Function called every frame on this Actor. For custom logic, Epic updates speed and gear of the pawn vehicle.
	 *
	 * @param Delta							Game time elapsed during last frame, modified by the time dilation
	 * @note Seems like PrimaryActorTick.bCanEverTick = true is no longer needed in 5.4 (against what is documented in AActor class)?
	 */ 
	virtual void Tick(float Delta) override;

	// End Actor interface

	// Begin PlayerController interface
protected:

	/**
	 * @brief Overridden native function for when this controller is asked to possess a pawn.
	 *
	 * Stores reference to the controlled vehicle pawn
	 *
	 * @param InPawn						The Pawn to be possessed
	 */ 
	virtual void OnPossess(APawn* InPawn) override;

	// End PlayerController interface
};

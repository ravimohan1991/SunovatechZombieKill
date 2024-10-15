/**
 * @file SunovatechZombieKillUI.h
 * @author Ravi Mohan
 * @brief This file contains native UI code for HUD type display. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 17, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SunovatechZombieKillUI.generated.h"

/**
 * @brief Simple Vehicle HUD class
 *
 * Displays the current speed and gear.
 * Widget setup is handled in a Blueprint subclass.
 *
 * @note We have a proper ASunovatechZombieKillHUD class for the purpose
 * @todo Ponder over moving this code to the proper HUD class if appropriate
 */
UCLASS(abstract)
class SUNOVATECHZOMBIEKILL_API USunovatechZombieKillUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	/**
	 * @brief Controls the display of speed in Km/h or MPH
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicle)
	bool bIsMPH = false;

public:

	/**
	 * @brief Called to update the speed display (via blueprint implemented function OnSpeedUpdate)
	 *
	 * @param NewSpeed								The updated speed for display in UI
	 */
	void UpdateSpeed(float NewSpeed);

	/**
	 * @brief Called to update the gear display (via blueprint implemented function OnGearUpdate)
	 *
	 * @param NewGear								The updated gear for display in UI
	 */
	void UpdateGear(int32 NewGear);

protected:

	/**
	 * @brief Implemented in Blueprint to display the new speed
	 *
	 * @param NewSpeed								The updated speed for display in UI
	 *
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = Vehicle)
	void OnSpeedUpdate(float NewSpeed);

	/**
	 * @brief Implemented in Blueprint to display the new gear
	 *
	 * @param NewGear								The updated gear for display in UI
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = Vehicle)
	void OnGearUpdate(int32 NewGear);
};

/**
 * @file SunovatechZombieKillHUD.h
 * @author Ravi Mohan
 * @brief This file contains HUD for the vehicle. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 18, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SunovatechZombieKillHUD.generated.h"

/**
 * @brief Draw a basic reticle with HUD
 * 
 * @note Seems like from UE5 onwards, new way is to use UUserWidget class for HUD. In this 
 * project, see USunovatechZombieKillUI and blueprint version. I'll be using old style
 * HUD system for prototype purpose whilst attempting to learn the new way. Both AHUD
 * and UUserWidget are playing nicely for now.
 */
UCLASS()
class SUNOVATECHZOMBIEKILL_API ASunovatechZombieKillHUD : public AHUD
{
	GENERATED_BODY()

public:
	/**
	 * @brief A constructor to load up reticle resource and cache that
	 */
	ASunovatechZombieKillHUD();

	/**
	 * @brief Primary draw call for the HUD 
	 */
	virtual void DrawHUD() override;

	/**
	 * Getter for reticle coordinates
	 */
	 FVector2D& GetReticleCoordinates() { return ReticleCoordinates; }

private:
	/** 
	 * Crosshair asset pointer for caching
	 */
	class FTexture* CrosshairTex;

	/** Caching reticle position*/
	FVector2D ReticleCoordinates;

protected:
	/** Offset from center of screen for reticle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	FVector2D OffsetFromCenter;

	/**
	* The font of the text diaplayed on the HUD
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HUD)
	UFont* TextFont;

protected:
	/** 
	 * @brief Overridable native event for when play begins for this actor. 
	 */
	virtual void BeginPlay();
};

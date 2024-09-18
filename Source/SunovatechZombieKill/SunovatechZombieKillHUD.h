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
 */
UCLASS()
class SUNOVATECHZOMBIEKILL_API ASunovatechZombieKillHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASunovatechZombieKillHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class FTexture* CrosshairTex;

protected:
	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay();
};

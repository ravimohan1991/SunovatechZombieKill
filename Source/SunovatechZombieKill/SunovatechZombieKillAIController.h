/**
 * @file SunovatechZombieKillAIController.h
 * @author Ravi Mohan
 * @brief This file contains AIController for zombie class. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 20, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */
#pragma once

#include "CoreMinimal.h"
#include "SunovatechZombieKillStProjectile.h"
#include "SunovatechZombieKillPawn.h"
#include "AIController.h"

#include "SunovatechZombieKillAIController.generated.h"

class UBehaviorTreeComponent;

/**
 * @brief The AI for zombie 
 */
UCLASS()
class SUNOVATECHZOMBIEKILL_API ASunovatechZombieKillAIController : public AAIController
{
	GENERATED_BODY()

	/** Called whenever the controller possesses a zombie (bot or pawn) */
	virtual void OnPossess(class APawn* InPawn) override;

	/** Called when controller unpossess the zombie */
	virtual void OnUnPossess() override;

	UBehaviorTreeComponent* BehaviorComp;
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetEnemyKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName PatrolLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName CurrentWaypointKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName BotTypeKeyName;

public:
	ASunovatechZombieKillAIController();

	AActor* GetWaypoint() const;

	APawn* GetTargetEnemy() const;

	void SetWaypoint(AActor* NewWaypoint);

	void SetTargetEnemy(APawn* NewTarget);

	void SetBlackboardBotType(EBotBehaviorType NewType);

	/** Returns BehaviorComp subobject */
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	
};

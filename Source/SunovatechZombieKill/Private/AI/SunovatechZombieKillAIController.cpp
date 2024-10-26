#include "AI/SunovatechZombieKillAIController.h"
#include "SunovatechZombieKillZoCharacter.h"
/* AI Specific includes */
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ASunovatechZombieKillAIController::ASunovatechZombieKillAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	/* Match with the ZombieBlackboard */
	PatrolLocationKeyName = "PatrolLocation";
	CurrentWaypointKeyName = "CurrentWaypoint";
	BotTypeKeyName = "BotType";

	/* Initializes PlayerState so we can assign a team index to AI */
	// bWantsPlayerState = true;
}

void ASunovatechZombieKillAIController::OnPossess(class APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ASunovatechZombieKillZoCharacter* ZombieBot = Cast<ASunovatechZombieKillZoCharacter>(InPawn);
	if(ZombieBot && BehaviorComp && BlackboardComp)
	{
		if(ensure(ZombieBot->BehaviorTree->BlackboardAsset))
		{
			BlackboardComp->InitializeBlackboard(*ZombieBot->BehaviorTree->BlackboardAsset);
		}

		BehaviorComp->StartTree(*ZombieBot->BehaviorTree);

		/* Make sure the Blackboard has the type of bot we possessed*/
		SetBlackboardBotType(ZombieBot->BotType);
	}
}

void ASunovatechZombieKillAIController::OnUnPossess()
{
	Super::OnUnPossess();

	/* Stop any behavior running because controller no longer has a pawn to control */
	if(BehaviorComp)
	{
		BehaviorComp->StopTree();
	}
}

void ASunovatechZombieKillAIController::SetWaypoint(AActor* NewWaypoint)
{
	if(BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(CurrentWaypointKeyName, NewWaypoint);
	}
}

void ASunovatechZombieKillAIController::SetTargetEnemy(APawn* NewTarget)
{
	if(BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(TargetEnemyKeyName, NewTarget);
		if(NewTarget == NULL)
		{
			UE_LOG(LogSunovatechZombieKill, Log, TEXT("NewTarget is NULL"));
		}
	}
}

AActor* ASunovatechZombieKillAIController::GetWaypoint() const
{
	if(BlackboardComp)
	{
		return Cast<AActor>(BlackboardComp->GetValueAsObject(CurrentWaypointKeyName));
	}

	return nullptr;
}

APawn* ASunovatechZombieKillAIController::GetTargetEnemy() const
{
	if (BlackboardComp)
	{
		return Cast<APawn>(BlackboardComp->GetValueAsObject(TargetEnemyKeyName));
	}

	return nullptr;
}


void ASunovatechZombieKillAIController::SetBlackboardBotType(EBotBehaviorType NewType)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsEnum(BotTypeKeyName, (uint8)NewType);
	}
}
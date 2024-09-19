#include "SunovatechZombieKillZoCharacter.h"
//#include "Items/SDamageType.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
ASunovatechZombieKillZoCharacter::ASunovatechZombieKillZoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 100;

	TargetingSpeedModifier = 0.5f;
	SprintingSpeedModifier = 1.0f;

	/* Noise emitter for both players and enemies. This keeps track of MakeNoise data and is used by pawnsensing component. */
	NoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitterComp"));

	// Needs thinking
	/* Don't collide with camera checks to keep 3rd person camera at position when zombies or other players are standing behind us */
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	/* Note: We assign the Controller class in the Blueprint extension of this class
		Because the zombie AIController is a blueprint in content and it's better to avoid content references in code.  */
	// AIControllerClass = ASZombieAIController::StaticClass();

	/* Our sensing component to detect player (vehicle) by visibility and noise checks. */
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(60.0f);
	PawnSensingComp->SightRadius = 2000;
	PawnSensingComp->HearingThreshold = 600;
	PawnSensingComp->LOSHearingThreshold = 1200;
}

// Called when the game starts or when spawned
void ASunovatechZombieKillZoCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASunovatechZombieKillZoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASunovatechZombieKillZoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


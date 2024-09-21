#include "SunovatechZombieKillZoCharacter.h"
//#include "Items/SDamageType.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
// AI include
#include "Perception/PawnSensingComponent.h"
// AI End
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "SunovatechZombieKillAIController.h"

// Sets default values
ASunovatechZombieKillZoCharacter::ASunovatechZombieKillZoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASunovatechZombieKillZoCharacter::OnSeePlayer);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &ASunovatechZombieKillZoCharacter::OnHearNoise);

	/* Ignore this (Weapon Collision, needs thinking) channel or it will absorb the trace impacts instead of the skeletal mesh */
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_OverlapAll_Deprecated COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f, false);
	GetCapsuleComponent()->SetCapsuleRadius(42.0f);

	/* These values are matched up to the CapsuleComponent above and are used to find navigation paths */
	GetMovementComponent()->NavAgentProps.AgentRadius = 42;
	GetMovementComponent()->NavAgentProps.AgentHeight = 192;

	AudioLoopComp = CreateDefaultSubobject<UAudioComponent>(TEXT("ZombieLoopedSoundComp"));
	AudioLoopComp->bAutoActivate = false;
	AudioLoopComp->bAutoDestroy = false;
	AudioLoopComp->SetupAttachment(RootComponent);

	Health = 100;
	MeleeDamage = 24.0f;
	SprintingSpeedModifier = 2.0f;

	/* By default we will not let the AI patrol, we can override this value per-instance. */
	BotType = EBotBehaviorType::Passive;
	SenseTimeOut = 2.5f;

	/* Note: Visual Setup is done in the AI/ZombieCharacter Blueprint file */

}

// Called when the game starts or when spawned
void ASunovatechZombieKillZoCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* This is the earliest moment we can bind our delegates to the component */
	/*if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ASunovatechZombieKillZoCharacter::OnSeePlayer);
		PawnSensingComp->OnHearNoise.AddDynamic(this, &ASunovatechZombieKillZoCharacter::OnHearNoise);
	}*/

	BroadcastUpdateAudioLoop(bSensedTarget);

	/* Assign a basic name to identify zombies in the HUD if needed. */
	APlayerState* PS = GetPlayerState();
	if(PS)
	{
		PS->SetPlayerName("Zombie");
		PS->SetIsABot(true);
	}
}

// Called every frame
void ASunovatechZombieKillZoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* Check if the last time we sensed a player (vehicle) is beyond the time out value to prevent zombie from endlessly following a player. */
	if(bSensedTarget && (GetWorld()->TimeSeconds - LastSeenTime) > SenseTimeOut
		&& (GetWorld()->TimeSeconds - LastHeardTime) > SenseTimeOut)
	{
		ASunovatechZombieKillAIController* AIController = Cast<ASunovatechZombieKillAIController>(GetController());
		if(AIController)
		{
			bSensedTarget = false;

			/* Reset controller in effect*/
			AIController->SetTargetEnemy(nullptr);

			/* Stop playing the hunting sound */
			BroadcastUpdateAudioLoop(false);
		}
	}
}

void ASunovatechZombieKillZoCharacter::OnSeePlayer(APawn* Pawn)
{
	if(!IsAlive()) // hehe, zombies are undead but this should just work
	{
		return;
	}

	if(!bSensedTarget)
	{
		BroadcastUpdateAudioLoop(true);
	}

	/* Keep track of the time the player was last sensed in order to clear the target */
	LastSeenTime = GetWorld()->GetTimeSeconds();
	bSensedTarget = true;

	ASunovatechZombieKillAIController* AIController = Cast<ASunovatechZombieKillAIController>(GetController());
	if(Pawn && /*Pawn->isalive*/ AIController)
	{
		AIController->SetTargetEnemy(Pawn);
	}
}


void ASunovatechZombieKillZoCharacter::PerformMeleeStrike_Implementation(AActor* HitActor)
{
	// In our context, most likelay vehicle pawn
	if(HitActor && HitActor != this && IsAlive())
	{
		ACharacter* OtherPawn = Cast<ACharacter>(HitActor);
		if(OtherPawn)
		{
			// Some PlayerState logic MISSING, needs thinking

			/* Set to prevent a zombie to attack multiple times in a very short time */
			LastMeleeAttackTime = GetWorld()->GetRealTimeSeconds();
			
			FPointDamageEvent DamageEvent;
			DamageEvent.DamageTypeClass = PunchDamageType;
			DamageEvent.Damage = MeleeDamage;

			HitActor->TakeDamage(DamageEvent.Damage, DamageEvent, GetController(), this);
			PlayCharacterSound(SoundAttackMelee);
		}
	}
}

void ASunovatechZombieKillZoCharacter::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	if(!IsAlive())
	{
		return;
	}

	if(!bSensedTarget)
	{
		BroadcastUpdateAudioLoop(true);
	}

	bSensedTarget = true;
	LastHeardTime = GetWorld()->GetTimeSeconds();

	ASunovatechZombieKillAIController* AIController = Cast<ASunovatechZombieKillAIController>(GetController());
	if(AIController)
	{
		AIController->SetTargetEnemy(PawnInstigator);
	}
}

void ASunovatechZombieKillZoCharacter::SetBotType(EBotBehaviorType NewType)
{
	BotType = NewType;

	ASunovatechZombieKillAIController* AIController = Cast<ASunovatechZombieKillAIController>(GetController());
	if(AIController)
	{
		AIController->SetBlackboardBotType(NewType);
	}

	BroadcastUpdateAudioLoop(bSensedTarget);
}

UAudioComponent* ASunovatechZombieKillZoCharacter::PlayCharacterSound(USoundCue* CueToPlay)
{
	if(CueToPlay)
	{
		return UGameplayStatics::SpawnSoundAttached(CueToPlay, RootComponent, NAME_None, FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
	}

	return nullptr;
}

void ASunovatechZombieKillZoCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if(PawnInstigator == LastTakeHitInfo.PawnInstigator.Get() && LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass)
	{
		// Same frame damage
		if(bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, ignore it
			return;
		}

		DamageTaken += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = DamageTaken;
	LastTakeHitInfo.PawnInstigator = Cast<ASunovatechZombieKillZoCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	//LastTakeHitInfo.EnsureReplication();

	if (bKilled && SoundDeath)
	{
		UGameplayStatics::SpawnSoundAttached(SoundDeath, RootComponent, NAME_None, FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
	}
	else if (SoundTakeHit)
	{
		UGameplayStatics::SpawnSoundAttached(SoundTakeHit, RootComponent, NAME_None, FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
	}

	/* Stop playing the hunting sound */
	if(AudioLoopComp && bKilled)
	{
		AudioLoopComp->Stop();
	}
}

bool ASunovatechZombieKillZoCharacter::IsSprinting() const
{
	/* Make zombie sprint when they see vehicle pawn. */
	return bSensedTarget && !GetVelocity().IsZero();
}

void ASunovatechZombieKillZoCharacter::BroadcastUpdateAudioLoop_Implementation(bool bNewSensedTarget)
{
	/* Start playing the hunting sound and the "noticed player" sound if the state is about to change */
	if (bNewSensedTarget && !bSensedTarget)
	{
		PlayCharacterSound(SoundPlayerNoticed);

		AudioLoopComp->SetSound(SoundHunting);
		AudioLoopComp->Play();
	}
	else
	{
		if (BotType == EBotBehaviorType::Patrolling)
		{
			AudioLoopComp->SetSound(SoundWandering);
			AudioLoopComp->Play();
		}
		else
		{
			AudioLoopComp->SetSound(SoundIdle);
			AudioLoopComp->Play();
		}
	}
}

void ASunovatechZombieKillZoCharacter::ServerSetSprinting_Implementation(bool NewSprinting)
{
	SetSprinting(NewSprinting);
}

float ASunovatechZombieKillZoCharacter::GetHealth() const
{
	return Health;
}

float ASunovatechZombieKillZoCharacter::GetMaxHealth() const
{
	// Retrieve the default value of the health property that is assigned on instantiation.
	return GetClass()->GetDefaultObject<ASunovatechZombieKillZoCharacter>()->Health;
}

// Refactor with more appropriate name for zombie
bool ASunovatechZombieKillZoCharacter::IsAlive() const
{
	return Health > 0;
}

float ASunovatechZombieKillZoCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if(Health <= 0.f)
	{
		return 0.f;
	}

	/* Modify based on gametype rules. Mutator type functionality, maybe for future */
	//AGameMode* MyGameMode = Cast<AGameMode>(GetWorld()->GetAuthGameMode());
	//Damage = MyGameMode ? MyGameMode->Modify
	
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if(ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if(Health <= 0)
		{
			//bool bCanDie = true;
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			APawn* Pawn = EventInstigator ? EventInstigator->GetPawn() : nullptr;
			PlayHit(ActualDamage, DamageEvent, Pawn, DamageCauser, false);
		}
	}

	return ActualDamage;
}

bool ASunovatechZombieKillZoCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	/* Check if character is already dying, destroyed or if we have authority (for multiplayer maybe) */
	if(bIsDying || !IsValid(this) || /*!HasAuthority() ||*/ GetWorld()->GetAuthGameMode() == nullptr)
	{
		return false;
	}

	return true;
}

void ASunovatechZombieKillZoCharacter::FellOutOfWorld(const class UDamageType& DamageType)
{
	Die(Health, FDamageEvent(DamageType.GetClass()), nullptr, nullptr);
}

// Fun stuff, killing zombie
bool ASunovatechZombieKillZoCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if(!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, Health);

	/* Fallback to default DamageType if noone is specified */
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);// mostly vehicle pawn controller

	// Maybe for future if game complexity grows
	/* Notify the gamemode we got killed for scoring and game over state */
	//AController* KilledPlayer = Controller ? Controller : Cast<AController>(GetOwner());
	//GetWorld()->GetAuthGameMode<ASGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}

void ASunovatechZombieKillZoCharacter::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	if(bIsDying)
	{
		return;
	}

	bIsDying = true;

	PlayHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

	DetachFromControllerPendingDestroy();

	/* Disable all collision on capsule */
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	USkeletalMeshComponent* Mesh3P = GetMesh();
	if(Mesh3P)
	{
		Mesh3P->SetCollisionProfileName(TEXT("Ragdoll"));
	}

	SetActorEnableCollision(true);

	// Karma, hehe.
	SetRagdollPhysics();

	/* Apply physics impulse on the bone of the enemy skeleton mesh we hit (ray-trace damage only) */
	if(DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent PointDamage = *((FPointDamageEvent*)(&DamageEvent));
		{
			Mesh3P->AddImpulseAtLocation(PointDamage.ShotDirection * 1200, PointDamage.HitInfo.ImpactPoint, PointDamage.HitInfo.BoneName);
		}
	}
	if(DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent RadialDamage = *((FRadialDamageEvent const*)(&DamageEvent));
		{
			Mesh3P->AddRadialImpulse(RadialDamage.Origin, RadialDamage.Params.GetMaxRadius(), 100000 /* RadialDamage.DamageTypeClass->DamageImpulse*/, ERadialImpulseFalloff::RIF_Linear);
		}
	}
}

void ASunovatechZombieKillZoCharacter::SetRagdollPhysics()
{
	bool bInRagdoll;
	USkeletalMeshComponent* Mesh3P = GetMesh();

	if(!IsValid(this))
	{
		bInRagdoll = false;
	}
	else if(!Mesh3P || !Mesh3P->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}
	else
	{
		Mesh3P->SetAllBodiesSimulatePhysics(true);
		Mesh3P->SetSimulatePhysics(true);
		Mesh3P->WakeAllRigidBodies();
		Mesh3P->bBlendPhysics = true;

		bInRagdoll = true;
	}

	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if(CharacterComp)
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}

	if(!bInRagdoll)
	{
		// Immediately hide the pawn
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else
	{
		SetLifeSpan(10.0f);
	}
}

void ASunovatechZombieKillZoCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get(), LastTakeHitInfo.bKilled);
	}
}

void ASunovatechZombieKillZoCharacter::SetSprinting(bool NewSprinting)
{
	bWantsToRun = NewSprinting;

	if(bIsCrouched)
	{
		UnCrouch();
	}

	//SetSprinting(NewSprinting);
}

float ASunovatechZombieKillZoCharacter::GetSprintingSpeedModifier() const
{
	return SprintingSpeedModifier;
}

void ASunovatechZombieKillZoCharacter::SetTargeting(bool NewTargeting)
{
	bIsTargeting = NewTargeting;

	//ServerSetTargeting(NewTargeting);
}

void ASunovatechZombieKillZoCharacter::ServerSetTargeting_Implementation(bool NewTargeting)
{
	SetTargeting(NewTargeting);
}

bool ASunovatechZombieKillZoCharacter::IsTargeting() const
{
	return bIsTargeting;
}

float ASunovatechZombieKillZoCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}

FRotator ASunovatechZombieKillZoCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

void ASunovatechZombieKillZoCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Value is already updated locally, skip in replication step
	//DOREPLIFETIME_CONDITION(ACharacter, bWantsToRun, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(ACharacter, bIsTargeting, COND_SkipOwner);

	// Replicate to every client, no special condition required
	//DOREPLIFETIME(ACharacter, Health);
	//DOREPLIFETIME(ACharacter, LastTakeHitInfo);
}

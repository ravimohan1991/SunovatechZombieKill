/**
 * @file SunovatechZombieKillZoCharacter.h
 * @author Ravi Mohan
 * @brief This file contains zombie class. The file was taken from Tom Looman's https://github.com/tomlooman/EpicSurvivalGame and modified by author.
 * @version 1.0
 * @date September 19, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */

#pragma once

#include "CoreMinimal.h"
#include "SunovatechZombieKillPawn.h"
#include "GameFramework/Character.h"
#include "SunovatechZombieKillStProjectile.h"

#include "SunovatechZombieKillZoCharacter.generated.h"

class USoundCue;

/**
 * @brief The zombie character class
 *
 * The following are contained
 * 1. PawnSensingComp - for seeing and hearing enemy
 * 2. Health
 * 3. Hit detection and effects like ragdoll on death
 * 4. Behavior Tree
 * 5. Sound cues
 */
UCLASS()
class SUNOVATECHZOMBIEKILL_API ASunovatechZombieKillZoCharacter : public ACharacter
{
	GENERATED_BODY()

	/**
	 * @brief Tracks noise data used by the pawn sensing component
	 */
	UPawnNoiseEmitterComponent* NoiseEmitterComp;

	/**
	 * @brief Last time the player was spotted
	 */
	float LastSeenTime;

	/**
	 * @brief Last time the player was heard
	 */
	float LastHeardTime;

	/**
	 * @brief Last time we attacked something
	 */
	float LastMeleeAttackTime;

	/**
	 * @brief Time-out value to clear the sensed position of the player. Should be higher than Sense interval in the PawnSense component not never miss sense ticks.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SenseTimeOut;

	/**
	 * @brief SensingComponent encapsulates sensory (ie sight and hearing) settings and functionality for this Actor,
	 * allowing the actor to see/hear Pawns in the world.
	 */
	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

public:
	/**
	 * @brief Sets default values for this character's properties.
	 *
	 * They include:
	 * 	1. Setting pawn sensing component
	 * 	2. Noise emitter component
	 * 	3. Set up collision
	 * 	4. Set up audio loop component (needs work in this project, if required)
	 * 	5. Variable initialization
	 */
	ASunovatechZombieKillZoCharacter();

	/**
	 * @brief The sound to be played when this zombie is hit
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundTakeHit;

	/**
	 * @brief The sound to be played when zombie is destroyed
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundDeath;

	/************************************************************************/
	/* Health                                                               */
	/************************************************************************/

	/**
	 * @brief Getter for initialized health
	 *
	 * @return Initial health
	 */
	UFUNCTION(BlueprintCallable, Category = "ZombieSituation")
	float GetMaxHealth() const;

	/**
	 * @brief Getter for current health
	 *
	 * @return Current health
	 */
	UFUNCTION(BlueprintCallable, Category = "ZombieSituation")
	float GetHealth() const;

	/**
	 * @brief Query about the state of zombie's being
	 *
	 * @return True if Health > 0, False otherwise
	 * @return Ponder over the possiblility of adding Health component instead for ASunovatechZombieKillZoCharacter and ASunovatechZombieKillPawn
	 */
	UFUNCTION(BlueprintCallable, Category = "ZombieSituation")
	bool IsAlive() const;

	/**
	 * @brief Query about the state of zombie's motion
	 *
	 * @return True if enemy has been sensed and zombie has started movement, else, False
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsSprinting() const;

	/**
	 * @brief Getter for attack melee status
	 */
	UFUNCTION(BlueprintCallable, Category = "ZombieSituation")
	bool IsMeleeAttacking() const { return bIsMeleeAttacking; }

	/**
	 * @brief Sets the attack status of Zombie
	 * 
	 * @todo ATM ASunovatechZombieKillPawn is detecting the overlapwith zombie and applying damage
	 * on self. Should be done in this class with applying of point damage.
	 */
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetAttack(bool bStatus) { bIsMeleeAttacking = bStatus; }

	/**
	 * @brief Is player aiming down sights (no practical use, in the project)
	 */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool IsTargeting() const;

	/**
	 * @brief Retrieve Pitch/Yaw from current camera (no practical use, in the project)
	 */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

	/**
	 * @brief Client/local call to update sprint state (no practical use, in the project)
	 */
	virtual void SetSprinting(bool NewSprinting);

	/**
	 * @brief Getter for SprintingSpeedModifier (IDK use)
	 *
	 * @brief SprintingSpeedModifier
	 */
	float GetSprintingSpeedModifier() const;

	/**
	 * @brief Getter for TargetingSpeedModifier (IDK use)
	 *
	 * @brief TargetingSpeedModifier
	 */
	float GetTargetingSpeedModifier() const;

	/**
	 * @brief Change default bot type during gameplay (not currently using this)
	 *
	 * @param NewType								The passive or patroling type behavior to be set
	 */
	void SetBotType(EBotBehaviorType NewType);

public:

	/**
	 * @brief Container for zombie's state of motion
	 */
	UPROPERTY(BlueprintReadOnly, Category = "AI")
	EZombieMotion ZState;

	/**
	 * @brief The bot behavior we want this bot to execute, (passive/patrol) by specifying EditAnywhere
	 * we can edit this value per-instance when placed on the map.
	 */
	UPROPERTY(EditAnywhere, Category = "AI")
	EBotBehaviorType BotType;

	/**
	 * @brief The thinking part of the brain, steers our zombie and makes decisions based on the data we feed it from the Blackboard
	 *
	 * @note Assigned at the Character level (instead of Controller) so we may use different zombie behaviors while re-using one controller.
	 */
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;

protected:

	/**
	 * @brief Is zombie attacking by punching (melee)
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Attacking")
	bool bIsMeleeAttacking;

	/**
	 * @brief Resets after sense time-out to avoid unnecessary clearing of target each tick
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	bool bSensedTarget;

	/**
	 * @brief The zombie sprint multiplier.
	 *
	 * @note Not being used in the project
	 * @todo Initialized twice in ASunovatechZombieKillZoCharacter::ASunovatechZombieKillZoCharacter(). Rectify by choosing one.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintingSpeedModifier;

	/**
	 * @brief Character wants to run, checked during Tick to see if allowed (not being used in the project)
	 */
	UPROPERTY(Transient)
	bool bWantsToRun;

	/**
	 * @brief No clue
	 */
	UPROPERTY(Transient)
	bool bIsTargeting;

	/**
	 * @brief No clue
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeedModifier;

	/** 
	 * @brief Holds hit data to replicate hits and death to clients. Also used for detecting same frame multiple hits
	 *
	 * @note We are not doing multiplayer yet. 
	 */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	/**
	 * @brief Current health of zombie
	 */
	UPROPERTY(EditDefaultsOnly, Category = "ZombieSituation")
	float Health;

	/**
	 * @brief The type of damage for zombie's punch (not in use)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	TSubclassOf<UDamageType> PunchDamageType;

	/**
	 * @brief The amount of damage during zombie's melee attack
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Attacking (Interaction)")
	float MeleeDamage;

	/**
	 * @brief The animation to be played whilst melee attack is active (not in use)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	UAnimMontage* MeleeAnimMontage;

	/************************************************************************/
	/* Sounds                                                               */
	/************************************************************************/

	/**
	 * @brief Sound to be played on spotting player (not in use)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundPlayerNoticed;

	/**
	 * @brief Zombie sound when sensed the target (not in use)
	 *
	 * @note May become functional once I figure out BroadcastUpdateAudioLoop in this project
	 * @see ASunovatechZombieKillZoCharacter::BroadcastUpdateAudioLoop_Implementation
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundHunting;

	/**
	 * @brief Zombie sound when EBotBehaviorType::Passive == BotType is true (not in use)
	 *
	 * @see BroadcastUpdateAudioLoop_Implementation
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundIdle;

	/**
	 * @brief Zombie sound when BotType is set to EBotBehaviorType::Patrolling (not in use)
	 *
	 * @see BroadcastUpdateAudioLoop_Implementation
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundWandering;

	/**
	 * @brief Zombie sound when melee action is being executed (not in use)
	 *
	 * @see ASunovatechZombieKillZoCharacter::PerformMeleeStrike
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundAttackMelee;

	/**
	 * @brief Plays the idle, wandering, or hunting sound
	 */
	UPROPERTY(VisibleAnywhere, Category = "Sound")
	UAudioComponent* AudioLoopComp;

	/**
	 * @brief True if OnDeath is called (need to check this)
	 *
	 * @see ASunovatechZombieKillZoCharacter::OnDeath
	 */
	bool bIsDying;

protected:

	/**
	 * @brief A native event for when play begins for this actor
	 *
	 * Setting up state for zombie
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Server side call to update actual sprint state yet not multiplayer.
	 *
	 * @note Find appropriate naming for the function
	 * @todo Naming something more appropriate
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GamePlay")
	void ServerSetSprinting(bool NewSprinting);

	/**
	 * @brief Not sure what this does
	 *
	 * @todo Think about the relevance of this function
	 */
	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	/**
	 * @brief Triggered by pawn sensing component when a (vehicle) pawn is spotted (sighted).
	 *
	 * @param Pawn								The spotted pawn
	 *
	 * @note Delegates can call member functions on C++ objects in a generic, type-safe way. A delegate can be bound dynamically to a member function
	 * of an arbitrary object, calling the function on the object at a future time, even if the caller does not know the object's type
	 * @note When using functions to attach to delegates, in our case UPawnSensingComponent::OnSeePawn, they need to be marked with UFUNCTION(). We assign this function to FSeePawnDelegate OnSeePawn
	 */
	UFUNCTION()
	void OnSeePlayer(APawn* Pawn);

	/**
	 * @brief Triggered by pawn sensing component when a (vehicle) pawn is heard (or noise from a vehicle Pawn's PawnNoiseEmitterComponent is heard)
	 *
	 * @param PawnInstigator								The pawn emitting the sound
	 * @param Location										Location where sound happened
	 * @param Volume										The intensity of sound (decibles?)
	 */
	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

	/**
	 * @brief Deal damage to the Actor that was hit by the punch animation of the zombie
	 *
	 * @param HitActor										The actor to be hurt
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attacking")
	void PerformMeleeStrike(AActor* HitActor);

	/**
	 * @brief Update the vocal loop of the zombie (idle, wandering, hunting) (not functional atm)
	 *
	 * @brief bNewSensedTarget								True when called from OnSeePlayer and OnHearNoise or bSensedTarget is true
	 *
	 * @todo Needs pondering over
	 * @todo To be made functional with single player in mind
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameExperience")
	void BroadcastUpdateAudioLoop(bool bNewSensedTarget);

	/**
	 * @brief Routine to play the cue sound for zombie's multiple states (idle, hunting, wandering, and attack etc)
	 *
	 * @param CueToPlay											The sound to be played
	 */
	UAudioComponent* PlayCharacterSound(USoundCue* CueToPlay);

	/************************************************************************/
	/* Targeting                                                            */
	/************************************************************************/

	/**
	 * @brief Sets the bIsTargetting, which I have not clue about
	 *
	 * @todo Needs removal in the current state of the project
	 */
	void SetTargeting(bool NewTargeting);

	/**
	 * @brief No clue because bIsTargetting's role is not known
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GamePlay")
	void ServerSetTargeting(bool NewTargeting);

	/**
	 * @brief The implementation function, which I don't think is necessary
	 */
	void ServerSetTargeting_Implementation(bool NewTargeting);

	/**
	 * @brief Take damage & handle death
	 *
	 * @param DamageAmount							How much damage to apply
	 * @param DamageEvent							Data package that fully describes the damage received.
	 * @param EventInstigator						The Controller responsible for the damage.
	 * @param DamageCauser							The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on zombie)
	 *
	 * @return										The amount of damage actually applied.
	 */
	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

	/**
	 * @brief Returns true if not already dying, else false
	 *
	 * @param KillingDamage							The amount of damage actually applied which led to the kill
	 * @param DamageEvent							Data package that fully describes the damage received.
	 * @param Killer								The controller responsible for the kill
	 * @param DamageCauser							The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on zombie)
	 *
	 * @return true or false depending upon condition
	 * @todo Remove virtual keyword
	 */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	/**
	 * @brief Set the health to zero and call ASunovatechZombieKillZoCharacter::OnDeath
	 *
	 * @param KillingDamage							The amount of damage actually applied which led to the kill
	 * @param DamageEvent							Data package that fully describes the damage received.
	 * @param Killer								The controller responsible for the kill
	 * @param DamageCauser							The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on zombie)
	 *
	 * @return true on successful death, else, false
	 * @todo Remove virtual keyword
	 */
	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	/**
	 * @brief Does the zombie mesh manipulations
	 *
	 * The following things are done
	 * 1. ASunovatechZombieKillZoCharacter::PlayHit is called
	 * 2. The owning controller is detatched
	 * 3. ASunovatechZombieKillZoCharacter::SetRagdollPhysics is called
	 * 4. Based on damage type, impulse is applied
	 *
	 * @todo Remove virtual keyword
	 */
	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);

	/**
	 * @brief Probably to be called if zombie fell out of world (IDK)
	 *
	 * @todo Remove virtual keyword
	 */
	virtual void FellOutOfWorld(const class UDamageType& DmgType);

	/**
	 * @brief Sets up mesh and components for ragdolling zombie
	 */
	void SetRagdollPhysics();

	/**
	 * @brief Routine to play the zombie hit logic concerned with damage
	 */
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Melee (using overlap)
	FORCEINLINE float GetMeleeDamage() const { return MeleeDamage; }

	UFUNCTION(BlueprintCallable, Category = "AI")
	FORCEINLINE EZombieMotion GetZombieMotionState() const { return ZState; }
};

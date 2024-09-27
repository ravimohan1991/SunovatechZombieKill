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
	UPROPERTY(BlueprintReadWrite, Category = "Attacking")
	bool bIsPunching;

	/* The bot behavior we want this bot to execute, (passive/patrol) by specifying EditAnywhere we can edit this value per-instance when placed on the map. */
	UPROPERTY(EditAnywhere, Category = "AI")
	EBotBehaviorType BotType;

	/* The thinking part of the brain, steers our zombie and makes decisions based on the data we feed it from the Blackboard */
	/* Assigned at the Character level (instead of Controller) so we may use different zombie behaviors while re-using one controller. */
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;

protected:
	/* Resets after sense time-out to avoid unnecessary clearing of target each tick */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	bool bSensedTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintingSpeedModifier;

	/* Character wants to run, checked during Tick to see if allowed */
	UPROPERTY(Transient)
	bool bWantsToRun;

	UPROPERTY(Transient)
	bool bIsTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeedModifier;

	/** 
	 * @brief Holds hit data to replicate hits and death to clients.
	 * 
	 * @note We are not doing multiplayer yet. 
	 */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	UPROPERTY(EditDefaultsOnly, Category = "ZombieSituation")
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	TSubclassOf<UDamageType> PunchDamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Attacking (Interaction)")
	float MeleeDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	UAnimMontage* MeleeAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundPlayerNoticed;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundHunting;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundIdle;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundWandering;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundAttackMelee;

	/* Plays the idle, wandering or hunting sound */
	UPROPERTY(VisibleAnywhere, Category = "Sound")
	UAudioComponent* AudioLoopComp;

	bool bIsDying;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Server side call to update actual sprint state yet not multiplayer. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GamePlay")
	void ServerSetSprinting(bool NewSprinting);

	//void ServerSetSprinting_Implementation(bool NewSprinting);

	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	/* Triggered by pawn sensing component when a pawn is spotted */
	/* When using functions as delegates they need to be marked with UFUNCTION(). We assign this function to FSeePawnDelegate */
	UFUNCTION()
	void OnSeePlayer(APawn* Pawn);

	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

	/* Deal damage to the Actor that was hit by the punch animation */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attacking")
	void PerformMeleeStrike(AActor* HitActor);

	//void PerformMeleeStrike_Implementation(AActor* HitActor);

	/* Update the vocal loop of the zombie (idle, wandering, hunting) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameExperience")
	void BroadcastUpdateAudioLoop(bool bNewSensedTarget);

	//void BroadcastUpdateAudioLoop_Implementation(bool bNewSensedTarget);

	UAudioComponent* PlayCharacterSound(USoundCue* CueToPlay);

	//void ServerSetSprinting_Implementation(bool NewSprinting);

	//bool ServerSetSprinting_Validate(bool NewSprinting);

	/************************************************************************/
	/* Targeting                                                            */
	/************************************************************************/

	void SetTargeting(bool NewTargeting);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GamePlay")
	void ServerSetTargeting(bool NewTargeting);

	void ServerSetTargeting_Implementation(bool NewTargeting);


	/* Take damage & handle death */
	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);

	virtual void FellOutOfWorld(const class UDamageType& DmgType);

	void SetRagdollPhysics();

	/**
	 * @brief Routine to play the zombie hit logic concerned with damage
	 */
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	//void ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Melee (using overlap)
	FORCEINLINE float GetMeleeDamage() { return MeleeDamage; }
};

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

UCLASS()
class SUNOVATECHZOMBIEKILL_API ASunovatechZombieKillZoCharacter : public ACharacter
{
	GENERATED_BODY()

	/* Tracks noise data used by the pawn sensing component */
	UPawnNoiseEmitterComponent* NoiseEmitterComp;

	/* Last time the player was spotted */
	float LastSeenTime;

	/* Last time the player was heard */
	float LastHeardTime;

	/* Last time we attacked something */
	float LastMeleeAttackTime;

	/* Time-out value to clear the sensed position of the player. Should be higher than Sense interval in the PawnSense component not never miss sense ticks. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SenseTimeOut;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

public:
	// Sets default values for this character's properties
	ASunovatechZombieKillZoCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundTakeHit;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundDeath;

	/************************************************************************/
	/* Health                                                               */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "ZombieSituation")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "ZombieSituation")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "ZombieSituation")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsSprinting() const;

	/* Is player aiming down sights */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool IsTargeting() const;

	/* Retrieve Pitch/Yaw from current camera */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;


	/* Client/local call to update sprint state  */
	virtual void SetSprinting(bool NewSprinting);

	float GetSprintingSpeedModifier() const;

	float GetTargetingSpeedModifier() const;

	/* Change default bot type during gameplay */
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

	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	//void ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Melee (using overlap)
	FORCEINLINE float GetMeleeDamage() { return MeleeDamage; }
};

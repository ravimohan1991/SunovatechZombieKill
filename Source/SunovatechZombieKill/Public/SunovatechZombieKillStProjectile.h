/**
 * @file SunovatechZombieKillStProjectile.h
 * @author Ravi Mohan
 * @brief This file contains the projectile class, thrown by the vehicle. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 18, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DamageEvents.h"
#include "SunovatechZombieKillStProjectile.generated.h"

 // Alternate of STypes.h
 // Think of finding appropriate place for these types

/**
 * @brief An enum for recording the zombie's state of motion
 */
UENUM(Blueprintable)
enum class EZombieMotion : uint8
{
	/**
	 * @brief Standing state
	 */
	Standing,

	/**
	 * @brief Walking state
	 */
	Walking,

	/**
	 * @brief Running state
	 */
	Running,
};

/**
 * @brief The player inventory enum
 *
 * @note These types have been taken from Tom Looman's https://github.com/tomlooman/EpicSurvivalGame project. Currently, our
 * project doesn't make use of these, but may use similar types in future.
 */
UENUM()
enum class EInventorySlot : uint8
{
	/**
	 * For currently equipped items/weapons
	 */
	Hands,

	/**
	 * For primary weapons on spine bone
	 */
	Primary,

	/**
	 * Storage for small items like flashlight on pelvis
	 */
	Secondary,
};

/**
 * @brief Enum for setting bot behavior
 *
 * @note Used by ASunovatechZombieController to set blackboard key, for now. Since we don't have patrolling set up, in the project, we
 * are not practically using this enum for gameplay.
 */
UENUM()
enum class EBotBehaviorType : uint8
{
	/**
	 * Does not move, remains in place until a player is spotted
	 */
	Passive,

	/**
	 * Patrols a region until a player is spotted
	 */
	Patrolling,
};

/**
 * @brief Useful structure for registering damagetype, amount, instigator, causer, and event class ID (point, general etc) when an AActor gets damaged.
 *
 * This structure is for complementing Engine's AActor::TakeDamage routine (https://www.unrealengine.com/en-US/blog/damage-in-ue4).
 * The rough (tentative) call stack is like so (when hit by a ASunovatechZombieKillStProjectile)
 *
 * 	1. ASunovatechZombieKillStProjectile::OnHit
 * 	2. UGameplayStatics::ApplyPointDamage
 * 	3. ASunovatechZombieKillZoCharacter::TakeDamage
 * 	4. ASunovatechZombieKillZoCharacter::PlayHit (<-- This function uses FTakeHitInfo which does nothing useful atm, in this
 * 	project)
 *
 * @note Probably we should use this for vehicle pawn damage logic too. ATM this structure has no practical use in this project (except perhaps to detect same frame damage)
 * @todo Gauge ASunovatechZombieKillZoCharacter::PlayHit and make necessary modification(s)
 *
 * @see ASunovatechZombieKillZoCharacter::PlayHit, ASunovatechZombieKillZoCharacter::TakeDamage
 */
USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

	/**
	 * @brief The total damage receieved by the AActor, in a frame, compounded if instigator and damagetype are same
	 * (ASunovatechZombieKillZoCharacter::PlayHit). Variable LastTakeHitInfo is used for compounding the damage.
	 *
	 * @see ASunovatechZombieKillZoCharacter::TakeDamage, ASunovatechZombieKillZoCharacter::PlayHit
	 */
	UPROPERTY()
	float ActualDamage;

	/**
	 * @brief The type of damage to be associated with  a damage event, interpreted by the receiving AActor, based on DamageEventClassID (corresponding to point, radial,
	 * or general damage event).
	 *
	 * @note If nullptr, UDamageType will be assumed.
	 * @see GetDamageEvent()
	 */
	UPROPERTY()
	UClass* DamageTypeClass;

	/**
	 * @brief The pawn responsible for this damage.
	 *
	 * ASunovatechZombieKillZoCharacter::TakeDamage -> EventInstigator->GetPawn()
	 */
	UPROPERTY()
	TWeakObjectPtr<class ACharacter> PawnInstigator;

	/**
	 * @brief The AActor that actually caused the damage
	 *
	 * ASunovatechZombieKillZoCharacter::TakeDamage -> DamageCauser
	 */
	UPROPERTY()
	TWeakObjectPtr<class AActor> DamageCauser;

	/**
	 * @brief The ID used to identify the type of damage event (point, radial, or general) from FDamageEvent
	 *
	 * @note Obtained from FDamageEvent::GetTypeID()
	 */
	UPROPERTY()
	uint8 DamageEventClassID;

	/**
	 * @brief If the damage led to killing event or death.
	 *
	 */
	UPROPERTY()
	bool bKilled;

private:

	UPROPERTY()
	uint8 EnsureReplicationByte;

	/**
	 * @brief A generic damage event for handling general damage events which are neither point damage events or radial damage events
	 */
	UPROPERTY()
	FDamageEvent GeneralDamageEvent;

	/**
	 * @brief A damage event for handling damage with a single impact location and source direction
	 */
	UPROPERTY()
	FPointDamageEvent PointDamageEvent;

	/**
	 * @brief A damage event for handling damage with a source location and falloff radius
	 */
	UPROPERTY()
	FRadialDamageEvent RadialDamageEvent;

public:

	/**
	 * @brief A constructor for 0 initialization
	 */
	FTakeHitInfo()
		: ActualDamage(0),
		DamageTypeClass(nullptr),
		PawnInstigator(nullptr),
		DamageCauser(nullptr),
		DamageEventClassID(0),
		bKilled(false),
		EnsureReplicationByte(0)
	{}

	/**
	 * @brief Routine to categorize the damage events (point and radial damage events, already defined in this structure) based on DamageEventClassID
	 *
	 * @return The interpreted damage event
	 */
	FDamageEvent& GetDamageEvent()
	{
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			if (PointDamageEvent.DamageTypeClass == nullptr)
			{
				PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return PointDamageEvent;

		case FRadialDamageEvent::ClassID:
			if (RadialDamageEvent.DamageTypeClass == nullptr)
			{
				RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return RadialDamageEvent;

		default:
			if (GeneralDamageEvent.DamageTypeClass == nullptr)
			{
				GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return GeneralDamageEvent;
		}
	}

	/**
	 * @brief Sets the supplied damage event to predefined damage events in this structure
	 *
	 * @param DamageEvent						The supplied damage event to be set
	 */
	void SetDamageEvent(const FDamageEvent& DamageEvent)
	{
		DamageEventClassID = DamageEvent.GetTypeID();
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			PointDamageEvent = *((FPointDamageEvent const*)(&DamageEvent));
			break;
		case FRadialDamageEvent::ClassID:
			RadialDamageEvent = *((FRadialDamageEvent const*)(&DamageEvent));
			break;
		default:
			GeneralDamageEvent = DamageEvent;
		}
	}

	void EnsureReplication()
	{
		EnsureReplicationByte++;
	}
};

/**
 * @brief A straight projectile which is fired by vehicle pawn to hit zombies
 */
UCLASS()
class SUNOVATECHZOMBIEKILL_API ASunovatechZombieKillStProjectile : public AActor
{
	GENERATED_BODY()

	/**
	 * @brief Sphere collision component
	 */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/**
	 * @brief Projectile movement component
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	
public:	

	/**
	 * @brief Sets default values for this actor's properties
	 */
	ASunovatechZombieKillStProjectile();

	/**
	 * @brief Called when bullet hits an Actor
	 *
	 * Useful to detect the collision events with zombies.
	 *
	 * @param HitComp									Pointer to the primitivecomponent (CollisionComp) of the projectile
	 * @param OtherActor								Pointer to the Actor that this projectile hit which are usually zombies
	 * @param OtherComp									Pointer to the primitivecomponent of the Other Actor
	 * @param NormalImpulse								The impusle vector imparted to Other Actor (zombie)
	 * @param Hit										Structure containing information about one hit of a trace, such as point of impact and
	 * 													surface normal at that point.
	 */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/**
	 * @brief Returns CollisionComp subobject
	 *
	 * @return CollisionComp
	 */
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/**
	 * @brief Returns ProjectileMovement subobject
	 *
	 * @return ProjectileMovement
	 */
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

public:
	/**
	 * @brief The amount of damage to be done by this projectile when hitting zombies
	 *
	 * @see ASunovatechZombieKillZoCharacter::TakeDamage
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float InflictedDamage;

	/**
	 * @brief Projectile hit effects (on zombies or walls or floor etc)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* ZombieHitEffect;

protected:

	/**
	 * @brief Overridable native event for when play begins for this actor.
	 */
	virtual void BeginPlay() override;
};

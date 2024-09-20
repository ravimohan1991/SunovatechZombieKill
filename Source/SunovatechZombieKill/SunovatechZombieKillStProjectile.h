/**
 * @file SunovatechZombieKillProjectile.h
 * @author Ravi Mohan
 * @brief This file contains the projectile class, thrown by the vehicle. The file was generated from UE by Epic Games and modified by author.
 * @version 1.0
 * @date September 18, 2024
 * @copyright Copyright Epic Games, Inc. All Rights Reserved. Ravi Mohan accepted UE(5) license.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SunovatechZombieKillStProjectile.generated.h"

UCLASS()
class SUNOVATECHZOMBIEKILL_API ASunovatechZombieKillStProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	
public:	
	// Sets default values for this actor's properties
	ASunovatechZombieKillStProjectile();

	/**
	 * Called when bullet hits an Actor
	 *
	 * Useful to detect the collision events with asteroids and spaceship.
	 *
	 * @param HitComp Pointer to the primitivecomponent of the asteroid
	 * @param OtherActor Pointer to the Actor hitting the asteroid
	 * @param OtherComp Pointer to the primitivecomponent of the Other Actor
	 * @param NormalImpulse
	 * @param Hit Structure containing information about one hit of a trace, such as point of impact and surface normal at that point.
	 */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

public:
	/** The amount of damage to be done by this projectile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float InflictedDamage;

	/** Zombie hit effects */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* ZombieHitEffect;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

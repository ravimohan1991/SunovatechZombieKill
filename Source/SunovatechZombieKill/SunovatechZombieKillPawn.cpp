// Copyright Epic Games, Inc. All Rights Reserved.

#include "SunovatechZombieKillPawn.h"
#include "SunovatechZombieKillWheelFront.h"
#include "SunovatechZombieKillWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "SunovatechZombieKill/SunovatechZombieKillStProjectile.h"
#include "SunovatechZombieKill/SunovatechZombieKillHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "SunovatechZombieKillZoCharacter.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

DEFINE_LOG_CATEGORY(LogTemplateVehicle);

ASunovatechZombieKillPawn::ASunovatechZombieKillPawn()
{
	// construct the front camera boom
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = false;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	// construct the back camera boom
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = true;
	BackSpringArm->bInheritRoll = true;
	BackSpringArm->bEnableCameraRotationLag = false;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));

	BackCamera->SetupAttachment(BackSpringArm);

	// Configure the car mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	// get the Chaos Wheeled movement component
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

	// Use a sphere as a simple collision representation
	OverlapComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	OverlapComp->InitSphereRadius(5.0f);
	OverlapComp->BodyInstance.SetCollisionProfileName("ZombieTouch");
	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &ASunovatechZombieKillPawn::OnOverlapBegin);
	OverlapComp->OnComponentEndOverlap.AddDynamic(this, &ASunovatechZombieKillPawn::OnOverlapEnd);

	// Set as root component
	RootComponent = OverlapComp;

	Health = 100;
	ZombiesAttacking = 0;
}

void ASunovatechZombieKillPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ASunovatechZombieKillPawn::Fire);
		//EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ASunovatechZombieKillPawn::Fire);

		// steering 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ASunovatechZombieKillPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ASunovatechZombieKillPawn::Steering);

		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASunovatechZombieKillPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASunovatechZombieKillPawn::Throttle);

		// break 
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ASunovatechZombieKillPawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ASunovatechZombieKillPawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ASunovatechZombieKillPawn::StopBrake);

		// handbrake 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ASunovatechZombieKillPawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ASunovatechZombieKillPawn::StopHandbrake);

		// look around 
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ASunovatechZombieKillPawn::LookAround);

		// toggle camera 
		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &ASunovatechZombieKillPawn::ToggleCamera);

		// reset the vehicle 
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ASunovatechZombieKillPawn::ResetVehicle);
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASunovatechZombieKillPawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// add some angular damping if the vehicle is in midair
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	// realign the camera yaw to face front
	//float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	//CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	//BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
}

void ASunovatechZombieKillPawn::Fire()
{
	//UE_LOG(LogTemp, Log, TEXT("Attempting fire"));

	// try and play the sound if specified
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (ProjectileClass)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(GetController());

			if(PlayerController && PlayerController->GetHUD())
			{
				ASunovatechZombieKillHUD* NativeHUD = Cast<ASunovatechZombieKillHUD>(PlayerController->GetHUD());

				if(NativeHUD)
				{
					FVector2D ReticleCoordinates = NativeHUD->GetReticleCoordinates();
					FVector FireDirection;
					FVector TempLocation;

					PlayerController->DeprojectScreenPositionToWorld(ReticleCoordinates.X, ReticleCoordinates.Y, TempLocation, FireDirection);

					const FRotator MuzzleRotation = FireDirection.Rotation(); //PlayerController->PlayerCameraManager->GetCameraRotation();
					const FVector MuzzleLocation = GetActorLocation() + MuzzleRotation.RotateVector(MuzzleOffset);

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
					ActorSpawnParams.Instigator = this;

					// spawn the projectile at the muzzle
					World->SpawnActor<ASunovatechZombieKillStProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
				}
			}
		}
	}
}

bool ASunovatechZombieKillPawn::IsAlive() const
{
	return Health > 0;
}

float ASunovatechZombieKillPawn::GetHealth() const
{
	return Health;
}

void ASunovatechZombieKillPawn::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASunovatechZombieKillZoCharacter* ZombiePawn = Cast<ASunovatechZombieKillZoCharacter>(OtherActor);
	if(ZombiePawn)
	{
		// Goccha
		UE_LOG(LogTemp, Log, TEXT("Zombie is overlapping pawn vehicle"));

		if(!GetWorld()->GetTimerManager().IsTimerActive(HurtTimerHandle))
		{
			FTimerHandle TimerHandle_AttackDelay = HurtTimerHandle;
			FTimerDelegate Delegate; // Delegate to bind function with parameters
			Delegate.BindUFunction(this, "PlayerHurt", ZombiePawn->GetMeleeDamage()); // Character is the parameter we wish to pass with the function.

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, 1.0f, true);
		}
		else
		{
			
		}
		ZombiesAttacking += 1;

		UE_LOG(LogTemp, Log, TEXT("Attacking zombie(s) %d"), ZombiesAttacking);

		//FTimerManagerTimerParameters InTimerParameters;
		//InTimerParameters.bLoop = true;

		//GetWorld()->GetTimerManager().SetTimer(HurtTimerHandle, &ASunovatechZombieKillPawn::PlayerHurt, 1.0f, InTimerParameters);
	}
}

void ASunovatechZombieKillPawn::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor && Cast<ASunovatechZombieKillZoCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Zombie has stopped overlapping pawn vehicle"));

		ZombiesAttacking -= 1;
		UE_LOG(LogTemp, Log, TEXT("Attacking zombie(s) %d"), ZombiesAttacking);

		if(ZombiesAttacking <= 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(HurtTimerHandle);
		}
	}
}

float ASunovatechZombieKillPawn::PlayerHurt(float DamageAmount)
{
	if (Health <= 0.f)
	{
		return 0.f;
	}

	if (DamageAmount > 0.f)
	{
		Health -= DamageAmount * ZombiesAttacking;
		UE_LOG(LogTemp, Log, TEXT("Hurting by the amount %f"), DamageAmount);

		if (Health <= 0)
		{
			// Handle death
		}
		else
		{
			// Not dead yet
		}
	}

	return DamageAmount;
}

float ASunovatechZombieKillPawn::GetMaxHealth() const
{
	// Retrieve the default value of the health property that is assigned on instantiation.
	return GetClass()->GetDefaultObject<ASunovatechZombieKillPawn>()->Health;
}

void ASunovatechZombieKillPawn::Steering(const FInputActionValue& Value)
{
	// get the input magnitude for steering
	float SteeringValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void ASunovatechZombieKillPawn::Throttle(const FInputActionValue& Value)
{
	// get the input magnitude for the throttle
	float ThrottleValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);
}

void ASunovatechZombieKillPawn::Brake(const FInputActionValue& Value)
{
	// get the input magnitude for the brakes
	float BreakValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetBrakeInput(BreakValue);
}

void ASunovatechZombieKillPawn::StartBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ASunovatechZombieKillPawn::StopBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(false);

	// reset brake input to zero
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ASunovatechZombieKillPawn::StartHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(true);

	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ASunovatechZombieKillPawn::StopHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(false);

	// call the Blueprint hook for the break lights
	BrakeLights(false);
}

void ASunovatechZombieKillPawn::LookAround(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	//UE_LOG(LogTemp, Log, TEXT("LookAxisVector is: (%f, %f)"), LookAxisVector.X, LookAxisVector.Y);

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void ASunovatechZombieKillPawn::ToggleCamera(const FInputActionValue& Value)
{
	// toggle the active camera flag
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

void ASunovatechZombieKillPawn::ResetVehicle(const FInputActionValue& Value)
{
	// reset to a location slightly above our current one
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	// reset to our yaw. Ignore pitch and roll
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;
	
	// teleport the actor to the reset spot and reset physics
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	UE_LOG(LogTemplateVehicle, Error, TEXT("Reset Vehicle"));
}

#undef LOCTEXT_NAMESPACE
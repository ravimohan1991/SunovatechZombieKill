#include "Vehicles/GoliathTankV.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "VehicleAnimationInstance.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SunovatechZombieKillHUD.h"
#include "SunovatechZombieKillStProjectile.h"
#include "GameFramework/SpringArmComponent.h"

AGoliathTankV::AGoliathTankV() : Super()
{
    TankGunMuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TankGunLocation"));
    TankGunMuzzleLocation->SetupAttachment(GetMesh());

    GunAngle = 0.f;
    GunElavation = 0.f;
}

void AGoliathTankV::BeginPlay()
{
    Super::BeginPlay();

    FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);

    // Attach gun muzzle location
    TankGunMuzzleLocation->AttachToComponent(GetMesh(), AttachRules, FName("gun_jntSocket"));

    // Attach back camera to gun joint
    GetBackSpringArm()->AttachToComponent(GetMesh(), AttachRules, FName("gun_jnt"));

    // Cache the animation bp of tank
    AnimationBP = Cast<UVehicleAnimationInstance>(GetMesh()->GetAnimInstance());
}

void AGoliathTankV::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // steering
	    EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AGoliathTankV::SteeringTrigger);
	    EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AGoliathTankV::SteeringComplete);
    }
}

void AGoliathTankV::SteeringTrigger(const FInputActionValue& Value)
{
    // Get the input value (magnitude) of steering
    float SteeringValue = Value.Get<float>();

    // Apply appropriate action
    ChaosVehicleMovement->SetYawInput(SteeringValue);
}

void AGoliathTankV::SteeringComplete(const FInputActionValue& Value)
{
    // Apply nullifying effect
    ChaosVehicleMovement->SetYawInput(0.f);
}

void AGoliathTankV::Tick(float Delta)
{
    Super::Tick(Delta);
}

FRotator AGoliathTankV::GetTurretOrientation(float InterpolateSpeed)
{
    FVector CameraForwardVector = GetBackCamera()->GetForwardVector();
    FVector Forward2D;
    Forward2D.X = CameraForwardVector.X;
    Forward2D.Y = CameraForwardVector.Y;
    Forward2D.Z = 0.f;

    Forward2D.Normalize();

    float CurrentElavation = 0.f;// = GetBackCamera()->GetComponentRotation().Pitch;//FMath::Atan(CameraForwardVector.Z / FMath::Sqrt(CameraForwardVector.X * CameraForwardVector.X + CameraForwardVector.Y * CameraForwardVector.Y)) * 180.f / 3.14f;
    
    //UE_LOG(LogSunovatechZombieKill, Log, TEXT("CurrentElavation: %f, Cached: %f"), CurrentElavation, GunElavation);
    //UE_LOG(LogSunovatechZombieKill, Log, TEXT("Setting initial Health to %f"), Health);

    // get the camera transform
	FVector CameraLoc = GetBackCamera()->GetComponentLocation();
	FRotator CameraRot = GetBackCamera()->GetComponentRotation();

	FVector Start = CameraLoc;
    // you need to add a uproperty to the header file for a float PlayerInteractionDistance
	FVector End = CameraLoc + (CameraRot.Vector() * PlayerInteractionDistance);

    TArray<AActor*> ActorsToIgnore;
    //ActorsToIgnore.Add(this);

	//  do the line trace
    FHitResult RV_Hit;
	bool DidTrace = UKismetSystemLibrary::LineTraceSingle(
        this,                                                                   //Worldcontext object
		Start,		                                                            //start
		End,		                                                            //end
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_PhysicsBody),	//Trace channel
        true,                                                                   //btracecomplex
        ActorsToIgnore,                                                         //Actors to ignore
        EDrawDebugTrace::None,                                                  //DebugTrace
        RV_Hit,                                                                 //HitResult
        true                                                                    //ignore self
		);

    if(DidTrace)
    {
        //UE_LOG(LogSunovatechZombieKill, Log, TEXT("Hitting [%s]"), *RV_Hit.GetActor()->GetName());
        //UE_LOG(LogSunovatechZombieKill, Log, TEXT("Hit Location (%f, %f, %f)"), RV_Hit.ImpactPoint.X, RV_Hit.ImpactPoint.Y, RV_Hit.ImpactPoint.Z);

        FVector GunSocketLocation = GetMesh()->GetSocketLocation(FName("gun_jnt"));
        float GunSocketHeight = GunSocketLocation.Z;

        FVector TowardsHitLocation = RV_Hit.ImpactPoint - GunSocketLocation;
        TowardsHitLocation.Normalize();

        float DotProduct = FVector::DotProduct(TowardsHitLocation, Forward2D);

        CurrentElavation = FMath::Acos(DotProduct) * 180.f / 3.14f;

        //UE_LOG(LogSunovatechZombieKill, Log, TEXT("CurrentElavation: %f"), CurrentElavation);

        if(TowardsHitLocation.Z <= 0.f)
        {
            CurrentElavation = -CurrentElavation;
        }
    }
    else
    {
        CurrentElavation = GunElavation;
    }

    CurrentElavation = FMath::Clamp(CurrentElavation, -6.f, 60.f);

    // Const vectors by definition
    FVector VectorForward;
    VectorForward.X = 1.f;
    VectorForward.Y = 0.f;
    VectorForward.Z = 0.f;

    FVector VectorRight;
    VectorRight.X = 0.f;
    VectorRight.Y = 1.f;
    VectorRight.Z = 0.f;

    float AngleWithForward = FMath::Acos(FVector::DotProduct(VectorForward, Forward2D)) * 180.f / 3.14f;

    if(FVector::DotProduct(VectorRight, Forward2D) <= 0.f)
    {
        AngleWithForward = 360.0f - AngleWithForward;
    }

    float CurrentAngleDiff = AngleWithForward - GetActorRotation().Yaw;

    FRotator FromRotation = FRotator::ZeroRotator;
    FRotator ToRotation = FRotator::ZeroRotator;

    FromRotation.Pitch = GunElavation;
    ToRotation.Pitch = CurrentElavation;

    FromRotation.Yaw = GunAngle;
    ToRotation.Yaw = CurrentAngleDiff;

    static float WorldDeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(this);

    return FMath::RInterpTo(FromRotation, ToRotation, WorldDeltaSeconds, InterpolateSpeed);
}

void AGoliathTankV::Fire()
{
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

					FRotator MuzzleRotation = FRotator::ZeroRotator;
                    MuzzleRotation.Yaw = GunAngle + GetActorRotation().Yaw;
                    MuzzleRotation.Pitch = GunElavation;

					const FVector MuzzleLocation = TankGunMuzzleLocation->GetComponentLocation();

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
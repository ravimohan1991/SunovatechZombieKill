// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicles/GoliathTankV.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "VehicleAnimationInstance.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

AGoliathTankV::AGoliathTankV() : Super()
{
    TankGunMuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TankGunLocation"));
    TankGunMuzzleLocation->SetupAttachment(GetMesh());

    GunAngle = 0.f;
}

void AGoliathTankV::BeginPlay()
{
    Super::BeginPlay();

    FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);

    // Attach gun muzzle location
    TankGunMuzzleLocation->AttachToComponent(GetMesh(), AttachRules, FName("gun_jntSocket"));

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

float AGoliathTankV::GetTurretAngle(float InterpolateSpeed)
{
    FVector CameraForwardVector = GetBackCamera()->GetForwardVector();
    FVector Forward2D;
    Forward2D.X = CameraForwardVector.X;
    Forward2D.Y = CameraForwardVector.Y;
    Forward2D.Z = 0.f;

    //UE_LOG(LogSunovatechZombieKill, Log, TEXT("%f, %f, %f"), Forward2D.X, Forward2D.Y, Forward2D.Z);

    Forward2D.Normalize();

   // UE_LOG(LogSunovatechZombieKill, Log, TEXT("%f, %f, %f"), Forward2D.X, Forward2D.Y, Forward2D.Z);

    FVector VectorForward;
    VectorForward.X = 1.f;
    VectorForward.Y = 0.f;
    VectorForward.Z = 0.f;

    FVector VectorRight;
    VectorRight.X = 0.f;
    VectorRight.Y = 1.f;
    VectorRight.Z = 0.f;

    float AngleWithForward = FMath::Acos(FVector::DotProduct(VectorForward, Forward2D)) * 180.f / 3.14f;

    //UE_LOG(LogSunovatechZombieKill, Log, TEXT("Angle: %f"), AngleWithForward);
    if(FVector::DotProduct(VectorRight, Forward2D) <= 0.f)
    {
        AngleWithForward = 360.0f - AngleWithForward;
    }

    float CurrentAngleDiff = AngleWithForward - GetActorRotation().Yaw;

    FRotator FromRotation = FRotator::ZeroRotator;
    FRotator ToRotation = FRotator::ZeroRotator;

    FromRotation.Yaw = GunAngle;
    ToRotation.Yaw = CurrentAngleDiff;

    //UE_LOG(LogSunovatechZombieKill, Log, TEXT("%f, %f, %f"), GetActorRotation().Pitch, GetActorRotation().Yaw, GetActorRotation().Roll);

    static float WorldDeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(this);

    return FMath::RInterpTo(FromRotation, ToRotation, WorldDeltaSeconds, InterpolateSpeed).Yaw;
}
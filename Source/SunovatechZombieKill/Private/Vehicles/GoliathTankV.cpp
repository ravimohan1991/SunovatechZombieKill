// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicles/GoliathTankV.h"

AGoliathTankV::AGoliathTankV() : Super()
{
    TankGunMuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TankGunLocation"));
    TankGunMuzzleLocation->SetupAttachment(GetMesh());
}
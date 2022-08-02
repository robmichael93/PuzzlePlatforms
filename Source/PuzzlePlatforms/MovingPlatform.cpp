// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = true;
    SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

    if(HasAuthority())
    {
        SetReplicates(true);
        SetReplicateMovement(true);
    }

    GlobalStartLocation = GetActorLocation();
    GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
}

void AMovingPlatform::AddActiveTrigger()
{
    ActiveTriggers++;
}

void AMovingPlatform::RemoveActiveTrigger()
{
    if (ActiveTriggers > 0)
    {
        ActiveTriggers--;
    }
}

void AMovingPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(HasAuthority() && ActiveTriggers > 0)
    {
        FVector Location = GetActorLocation();
        
        if ((Location - GlobalStartLocation).Size() >= (GlobalTargetLocation - GlobalStartLocation).Size())
        {
            FVector Temp;
            Temp = GlobalTargetLocation;
            GlobalTargetLocation = GlobalStartLocation;
            GlobalStartLocation = Temp;
        }
        
        FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
        Location += Direction * Speed * DeltaTime;
        SetActorLocation(Location);

    }
}

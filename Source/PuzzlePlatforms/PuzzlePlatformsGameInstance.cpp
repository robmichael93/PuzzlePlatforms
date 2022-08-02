// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"
#include "Engine\Engine.h"

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer)
{
//    UE_LOG(LogTemp, Warning, TEXT("Game instance constructor."));
}

void UPuzzlePlatformsGameInstance::Init()
{
//    UE_LOG(LogTemp, Warning, TEXT("Game instance Init."));    
}

void UPuzzlePlatformsGameInstance::Host()
{
/*     UEngine* Engine = GetEngine();

    if (Engine)
    {
        Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));
    }
 */
    UWorld* World = GetWorld();
    if (World)
    {
        World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
    }
}

void UPuzzlePlatformsGameInstance::Join(const FString &Address)
{
/*     UEngine* Engine = GetEngine();

    if (Engine)
    {
        Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));
    }
 */    
    APlayerController* Controller = GetFirstLocalPlayerController();
    if (Controller)
    {
        Controller->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
    }
}
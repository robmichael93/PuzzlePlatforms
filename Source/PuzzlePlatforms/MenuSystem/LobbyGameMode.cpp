// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "LobbyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"
#include "PuzzlePlatforms/PuzzlePlatformsGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ++NumberOfPlayers;
    if (NumberOfPlayers >= 3)
    {
        if (GameState != nullptr && GameState->PlayerArray.Num() > 0)
        {
            for (APlayerState* Player : GameState->PlayerArray)
            {
                if (Player)
                {
                    ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(Player->GetPlayerController());
                    if (PC)
                    {
                        PC->ShowLoadingScreen();
                    }
                }
            }
        }
        FTimerHandle LoadingScreenTimerHandle;
        GetWorldTimerManager().SetTimer(LoadingScreenTimerHandle, this, &ALobbyGameMode::LoadLevel, 5.f, false);
    }
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    --NumberOfPlayers;
}

void ALobbyGameMode::LoadLevel()
{
    UPuzzlePlatformsGameInstance* GameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        GameInstance->StartSession();
    }
    UWorld* World = GetWorld();
    if (World)
    {
        bUseSeamlessTravel = true;
        World->ServerTravel("/Game/Maps/Game?listen");
    }
}
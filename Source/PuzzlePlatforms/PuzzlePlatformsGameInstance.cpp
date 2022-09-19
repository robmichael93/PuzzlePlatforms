// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Online/CoreOnline.h"
#include "OnlineKeyValuePair.h"
#include "OnlineSessionSettings.h"
#include "PlatformTrigger.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/PopupMenu.h"
#include "UObject/ConstructorHelpers.h"

const static FName SESSION_NAME = FName("Puzzle Platforms Game Session");

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UMainMenu> MainMenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
    MenuClass = MainMenuBPClass.Class;
    ConstructorHelpers::FClassFinder<UPopupMenu> PopupMenuBPClass(TEXT("/Game/MenuSystem/WBP_PopupMenu"));
    PopupMenuClass = PopupMenuBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("Subsystem found: %s"), *Subsystem->GetSubsystemName().ToString());
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnSessionCreated);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnSessionDestroyed);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
        }
    }

    if (GEngine)
    {
        GEngine->OnNetworkFailure().AddUObject(this, &UPuzzlePlatformsGameInstance::OnNetworkFailure);
    }
}

void UPuzzlePlatformsGameInstance::LoadMenu()
{
    if (MenuClass)
    {
        Menu = CreateWidget<UMainMenu>(this, MenuClass);

        if (Menu)
        {
            Menu->Setup();
            Menu->SetMenuInterface(this);
        }
    }
    if (PopupMenuClass)
    {
        PopupMenu = CreateWidget<UPopupMenu>(this, PopupMenuClass);
        if (PopupMenu)
        {
            PopupMenu->SetPopupMenuInterface(this);
        }
    }
}

void UPuzzlePlatformsGameInstance::Host(FString InServerName)
{
    DesiredServerName = InServerName;

    if (SessionInterface.IsValid())
    {
        if (!SessionInterface->GetNamedSession(SESSION_NAME))
        {
            MakeNewSession();
        }
        else
        {
            SessionInterface->DestroySession(SESSION_NAME);
        }    
    }
}

void UPuzzlePlatformsGameInstance::MakeNewSession()
{
    if (SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Making a new session."));
        FOnlineSessionSettings SessionSettings;
        IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
        if (Subsystem)
        {
            if (Subsystem->GetSubsystemName().ToString() == "STEAM")
            {
            SessionSettings.bIsLANMatch = false;
            }
            else
            {
            SessionSettings.bIsLANMatch = true;
            }
        }
        SessionSettings.NumPublicConnections = 5;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bUseLobbiesIfAvailable = true;

        SessionSettings.Set(FName("ServerName"), DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineService);

        APlayerController* PlayerController = GetFirstLocalPlayerController();
        if (PlayerController)
        {
            APlayerState* PlayerState = PlayerController->PlayerState;
            if (PlayerState)
            {
                SessionSettings.Set(FName("HostName"), PlayerState->GetPlayerName(), EOnlineDataAdvertisementType::ViaOnlineService);
            }
        }

        SessionSettings.Set(FName("CurrentPlayers"), 1, EOnlineDataAdvertisementType::ViaOnlineService);
        SessionSettings.Set(FName("MaxPlayers"), SessionSettings.NumPublicConnections, EOnlineDataAdvertisementType::ViaOnlineService);
        SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);    
    }
}

void UPuzzlePlatformsGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
    UWorld* World = GetWorld();
    if (World && bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Session created successfully."));
        World->ServerTravel("/Game/Maps/Lobby?listen");
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Session not created successfully."));
    }
}

void UPuzzlePlatformsGameInstance::OnSessionDestroyed(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Session destroyed successfully."));
        MakeNewSession();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Session not destroyed successfully."));
    }
}

void UPuzzlePlatformsGameInstance::RefreshServers()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (SessionSearch.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Session search started."));
        IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
        if (Subsystem)
        {
            if (Subsystem->GetSubsystemName().ToString() == "STEAM")
            {
                SessionSearch->bIsLanQuery = false;
                SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
            }
            else
            {
                SessionSearch->bIsLanQuery = true;
            }
        }
        SessionSearch->MaxSearchResults = 100;
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }    
}

void UPuzzlePlatformsGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
    if (bWasSuccessful && SessionSearch.IsValid() && Menu)
    {
        UE_LOG(LogTemp, Warning, TEXT("Session search complete: %s"), (bWasSuccessful ? TEXT("true") : TEXT("false")));
        TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
        if (Results.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("No sessions found."));
            // Menu->SetServerList(Results); // temporarily used to trigger test server list for UI
        }
        else
        {
            TArray<FServerData> ServerListData;
            for (FOnlineSessionSearchResult Result : Results)
            {
                UE_LOG(LogTemp, Warning, TEXT("Session found: %s"), *Result.GetSessionIdStr());
                FOnlineSessionSettings SessionSettings = Result.Session.SessionSettings;

                FServerData Data;
                FString HostingServerName;
                SessionSettings.Get(FName("ServerName"), HostingServerName);
                Data.Name = HostingServerName;
                Data.Name.LeftInline(12);
                
                FString HostName;
                SessionSettings.Get(FName("HostName"), HostName);
                // HostName.LeftInline(8);
                Data.HostUserName = HostName;

                int NumCurrentPlayers = 0;
                SessionSettings.Get(FName("CurrentPlayers"), NumCurrentPlayers);
                Data.CurrentPlayers = NumCurrentPlayers;

                int32 MaxNumPlayers;
                SessionSettings.Get(FName("MaxPlayers"), MaxNumPlayers);
                Data.MaxPlayers = static_cast<uint16>(MaxNumPlayers);
                ServerListData.Add(Data);
            }

            Menu->CreateServerList(ServerListData);
        }
    }
}

void UPuzzlePlatformsGameInstance::StartSession()
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->StartSession(SESSION_NAME);
    }
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
    if (SessionInterface.IsValid() && SessionSearch.IsValid())
    {
        if (!SessionInterface->GetNamedSession(SESSION_NAME))
        {
            SelectedSession = SessionSearch->SearchResults[Index];
            SessionInterface->JoinSession(0, SESSION_NAME, SelectedSession);
        }
        else
        {
            FString ConnectInfo = "";
            SessionInterface->GetResolvedConnectString(SelectedSession, NAME_GamePort, ConnectInfo);
            APlayerController* Controller = GetFirstLocalPlayerController();
            if (Controller)
            {
                Controller->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
            }
        }
    }
}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        if (SessionInterface.IsValid() && SelectedSession.IsValid())
        {
            FString ConnectInfo = "";
            SessionInterface->GetResolvedConnectString(SelectedSession, NAME_GamePort, ConnectInfo);
            APlayerController* Controller = GetFirstLocalPlayerController();
            if (Controller)
            {
                Controller->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not join session."));
    }
}

void UPuzzlePlatformsGameInstance::OnNetworkFailure(UWorld * World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    if (World)
    {
        UGameplayStatics::OpenLevel(World, TEXT("/Game/Maps/MainMenu"));
    }    
}

void UPuzzlePlatformsGameInstance::PopupMenuToggle()
{
    if (PopupMenu)
    {
        if (bIsPopupMenuActive)
        {
            PopupMenu->RemoveFromViewport();

            UWorld* World = GetWorld();
            APlayerController* Controller = World->GetFirstPlayerController();
            if (Controller)
            {
                FInputModeGameOnly InputModeData;
                Controller->SetInputMode(InputModeData);
                Controller->bShowMouseCursor = false;
            }
        }
        else
        {
            PopupMenu->AddToViewport();

            UWorld* World = GetWorld();
            if (World)
            {
                APlayerController* Controller = GetWorld()->GetFirstPlayerController();
                if (Controller)
                {
                    FInputModeUIOnly InputModeData;
                    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                    Controller->SetInputMode(InputModeData);
                    Controller->bShowMouseCursor = true;
                }
            }
        }
        bIsPopupMenuActive = !bIsPopupMenuActive;
    }
}

void UPuzzlePlatformsGameInstance::QuitGame()
{
    bIsPopupMenuActive = false;
    if (PopupMenu)
    {
        PopupMenu->RemoveFromViewport();
    }
    UWorld* World = GetWorld();
    if (World)
    {
        UGameplayStatics::OpenLevel(World, TEXT("/Game/Maps/MainMenu"));
    }
    //     if (SessionInterface.IsValid())
    //     {
    //         IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    //         if (Subsystem)
    //         {
    //             TSharedPtr<const FUniqueNetId> ID = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
    //             if (ID.IsValid())
    //             {
    //                 bool Success = SessionInterface->UnregisterPlayer(SESSION_NAME, *ID.Get());
    //                 if (Success)
    //                 {
    //                     UE_LOG(LogTemp, Warning, TEXT("Successfully unregistered from session."));
    //                 }
    //                 else
    //                 {
    //                     UE_LOG(LogTemp, Warning, TEXT("Did not successfully unregistered from session."));
    //                 }
    //             }
    //         }
    //     }
}

void UPuzzlePlatformsGameInstance::Exit()
{
    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* Controller = GetFirstLocalPlayerController();
        if (Controller)
        {
            UKismetSystemLibrary::QuitGame(World, Controller, EQuitPreference::Quit, false);
        }
    }
}
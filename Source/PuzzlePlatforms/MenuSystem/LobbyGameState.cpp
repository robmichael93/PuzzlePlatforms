// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"

const static FName SESSION_NAME = FName("Puzzle Platforms Game Session");

void ALobbyGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsystem found: %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Interface found."));
			SessionInterface->OnUpdateSessionCompleteDelegates.AddUObject(this, &ALobbyGameState::OnUpdateCompleted);
		}
	}
	FTimerHandle SessionUpdateTimerHandle;
	GetWorldTimerManager().SetTimer(SessionUpdateTimerHandle, this, &ALobbyGameState::UpdateSession, 5.0f, true);
}

void ALobbyGameState::UpdateSession()
{
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* Session = SessionInterface->GetNamedSession(SESSION_NAME);

		if (Session)
		{
			UE_LOG(LogTemp, Warning, TEXT("Updating Session"));

			int NumCurrentPlayers = 0;
			if (PlayerArray.IsValidIndex(0) && PlayerArray.Num() > 0)
			{
				NumCurrentPlayers = PlayerArray.Num();
			}

			FOnlineSessionSettings* NewSettings = SessionInterface->GetSessionSettings(SESSION_NAME);
			NewSettings->Set(FName("CurrentPlayers"), NumCurrentPlayers, EOnlineDataAdvertisementType::ViaOnlineService);
			SessionInterface->UpdateSession(SESSION_NAME, *NewSettings, true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Session was not found while trying to update sessions."))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Interface was not found.  Trying to get it now."));
		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
		if (Subsystem)
		{
			UE_LOG(LogTemp, Warning, TEXT("Subsystem found: %s"), *Subsystem->GetSubsystemName().ToString());
			SessionInterface = Subsystem->GetSessionInterface();
			if (SessionInterface.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Session Interface found."));
				SessionInterface->OnUpdateSessionCompleteDelegates.AddUObject(this, &ALobbyGameState::OnUpdateCompleted);
			}
		}
	}    
}

void ALobbyGameState::OnUpdateCompleted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful && SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session update was successful: %s"), (bWasSuccessful ? TEXT("true") : TEXT("false")));
		FNamedOnlineSession* Session = SessionInterface->GetNamedSession(SessionName);

		if (Session)
		{
			FServerData Data;

			FString Server;
			Session->SessionSettings.Get(FName("ServerName"), Server);
			Data.Name = Server;

			FString Host;
			Session->SessionSettings.Get(FName("HostName"), Host);
			Data.HostUserName = Host;

			int NumCurrentPlayers;
			Session->SessionSettings.Get(FName("CurrentPlayers"), NumCurrentPlayers);
			Data.CurrentPlayers = NumCurrentPlayers;

			int NumMaxPlayers;
			Session->SessionSettings.Get(FName("MaxPlayers"), NumMaxPlayers);
			Data.MaxPlayers = NumMaxPlayers;

			UE_LOG(LogTemp, Warning, TEXT("Server name: %s"), *Data.Name);
			UE_LOG(LogTemp, Warning, TEXT("Host: %s"), *Data.HostUserName);
			UE_LOG(LogTemp, Warning, TEXT("Number of players: %d"), Data.CurrentPlayers);
			UE_LOG(LogTemp, Warning, TEXT("Max players: %d"), Data.MaxPlayers);

            OnServerDataChange.Broadcast(Data);

			// if (GameState != nullptr && GameState->PlayerArray.IsValidIndex(0) && GameState->PlayerArray.Num() > 0)
			// {
			// 	for (APlayerState* Player : GameState->PlayerArray)
			// 	{
			// 		if (Player)
			// 		{
			// 			ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(Player->GetPlayerController());
			// 			if (PC && PC->GetMainMenuRef())
			// 			{
			// 				PC->GetMainMenuRef()->UpdateSessionInfo(Data);
			// 			}
			// 		}
			// 	}
			// }
			// else
			// {
			// 	UE_LOG(LogTemp, Warning, TEXT("Game state or player array is not valid."));
			// }
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OnUpdateCompleted - Session was not found."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session update was successful: %s"), (bWasSuccessful ? TEXT("true") : TEXT("false")));
	}    
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MainMenu.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerDataChangeDelegate, FServerData, Data);

UCLASS()
class PUZZLEPLATFORMS_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()

public:
	FOnServerDataChangeDelegate OnServerDataChange;

	virtual void HandleMatchHasStarted() override;

private:
	IOnlineSessionPtr SessionInterface;
	
	void UpdateSession();

	void OnUpdateCompleted(FName SessionName, bool bWasSuccessful);
};

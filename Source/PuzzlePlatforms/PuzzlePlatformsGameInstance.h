// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MenuSystem/MenuInterface.h"
#include "MenuSystem/PopupMenuInterface.h"
#include "PuzzlePlatformsGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UPuzzlePlatformsGameInstance : public UGameInstance, public IMenuInterface, public IPopupMenuInterface
{
	GENERATED_BODY()
	
public:
	UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer);

	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable)
	void LoadMenu();

	UFUNCTION()
	void Host(FString InServerName) override;

	UFUNCTION()
	void Join(uint32 Index) override;

	UFUNCTION()
	void Exit() override;

	UFUNCTION()
	void RefreshServers() override;
	
	UFUNCTION()
	void PopupMenuToggle() override;

	UFUNCTION()
	void QuitGame() override;

	void StartSession();

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> MenuClass;

	UPROPERTY()
	class UMainMenu* Menu;

	UPROPERTY()
	TSubclassOf<UUserWidget> PopupMenuClass;

	UPROPERTY()
	class UPopupMenu* PopupMenu;

	UPROPERTY()
	bool bIsPopupMenuActive = false;

	UPROPERTY()
	FString DesiredServerName;

	IOnlineSessionPtr SessionInterface;
	
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FOnlineSessionSearchResult SelectedSession;

	void OnSessionCreated(FName SessionName, bool bWasSuccessful);

	void OnSessionDestroyed(FName SessionName, bool bWasSuccessful);

	void OnFindSessionComplete(bool bWasSuccessful);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnNetworkFailure(UWorld * World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString = TEXT(""));

	void MakeNewSession();
};

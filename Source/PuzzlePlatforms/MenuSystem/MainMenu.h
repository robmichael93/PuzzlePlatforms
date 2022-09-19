// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()
	
	FString Name;
	FString HostUserName;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
};

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UMainMenu(const FObjectInitializer & ObjectInitializer);

	void SetMenuInterface(IMenuInterface* NewMenuInterface);

	void Setup();

	void CreateServerList(TArray<FServerData> ServerListData);

	void SelectIndex(uint32 Index);

	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	UFUNCTION()
	void UpdateSessionInfo(FServerData Data);

protected:
	virtual bool Initialize();

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenuHostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenuCancelButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* HostNameInput;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinMenuCancelButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinMenuRefreshButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinMenuJoinButton;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ServerList;

	UPROPERTY()
	TSubclassOf<class UServerRow> ServerRowClass;

	UPROPERTY()
	class UServerRow* ServerRow;

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void ExitGame();
	
	UFUNCTION()
	void RefreshServerList();

	void UpdateChildren();
	
	IMenuInterface* MenuInterface;

	TOptional<uint32> SelectedIndex;
};

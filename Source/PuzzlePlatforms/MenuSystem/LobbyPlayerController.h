// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALobbyPlayerController();

	UFUNCTION(Client, Reliable)
	void ShowLoadingScreen();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void SetMainMenuRef(class UMainMenu* InMainMenu);

	UFUNCTION()
	class UMainMenu* GetMainMenuRef();

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> LoadingScreenClass;

	UPROPERTY()
	class UUserWidget* LoadingScreen;

	UPROPERTY()
	class UMainMenu* MainMenu;
};

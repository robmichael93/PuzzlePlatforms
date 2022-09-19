// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.h"

ALobbyPlayerController::ALobbyPlayerController()
{
    ConstructorHelpers::FClassFinder<UUserWidget> LoadingScreenBPClass(TEXT("/Game/MenuSystem/WBP_LoadingScreen"));
    LoadingScreenClass = LoadingScreenBPClass.Class;    
}

void ALobbyPlayerController::ShowLoadingScreen_Implementation()
{
    if (LoadingScreenClass)
    {
        LoadingScreen = CreateWidget(this, LoadingScreenClass);
        LoadingScreen->AddToViewport();
    }
}

void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (LoadingScreen)
    {
        LoadingScreen->RemoveFromViewport();
    }
}

void ALobbyPlayerController::SetMainMenuRef(UMainMenu* InMainMenu)
{
    MainMenu = InMainMenu;
}

UMainMenu* ALobbyPlayerController::GetMainMenuRef()
{
    return MainMenu;
}
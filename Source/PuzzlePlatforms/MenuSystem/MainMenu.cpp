// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "ServerRow.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.h"
#include "LobbyPlayerController.h"
#include "OnlineSessionSettings.h"
#include "UObject/ConstructorHelpers.h"

UMainMenu::UMainMenu(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UServerRow> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
    ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
    bool Success = Super::Initialize();
    if (Success)
    {
        if (HostButton)
        {
            HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);
        }
        else
        {
            return false;
        }
        if (JoinButton)
        {
            JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
        }
        else
        {
            return false;
        }
        if (ExitButton)
        {
            ExitButton->OnClicked.AddDynamic(this, &UMainMenu::ExitGame);
        }
        else
        {
            return false;
        }
        if (HostMenuHostButton)
        {
            HostMenuHostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
        }
        else
        {
            return false;
        }
        if (HostMenuCancelButton)
        {
            HostMenuCancelButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
        }
        else
        {
            return false;
        }
        if (JoinMenuCancelButton)
        {
            JoinMenuCancelButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
        }
        else
        {
            return false;
        }
        if (JoinMenuJoinButton)
        {
            JoinMenuJoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);
        }
        else
        {
            return false;
        }
        if (JoinMenuRefreshButton)
        {
            JoinMenuRefreshButton->OnClicked.AddDynamic(this, &UMainMenu::RefreshServerList);
        }
        else
        {
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}

void UMainMenu::SetMenuInterface(IMenuInterface* NewMenuInterface)
{
    MenuInterface = NewMenuInterface;
}

void UMainMenu::Setup()
{
    AddToViewport();

    UWorld* World = GetWorld();
    if (World)
    {
        ALobbyPlayerController* Controller = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());
        if (Controller)
        {
            FInputModeUIOnly InputModeData;
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            InputModeData.SetWidgetToFocus(TakeWidget());
            Controller->SetInputMode(InputModeData);
            Controller->bShowMouseCursor = true;

            Controller->SetMainMenuRef(this);
        }
        ALobbyGameState* GameState = Cast<ALobbyGameState>(World->GetGameState());

        if (GameState)
        {
            GameState->OnServerDataChange.AddDynamic(this, &UMainMenu::UpdateSessionInfo);
        }
    }
}

void UMainMenu::OpenMainMenu()
{
    if (MenuSwitcher)
    {
        if (MainMenu)
        {
            MenuSwitcher->SetActiveWidget(MainMenu);
        }
    }    
}

void UMainMenu::OpenHostMenu()
{
    if (MenuSwitcher)
    {
        if (HostMenu)
        {
            MenuSwitcher->SetActiveWidget(HostMenu);
        }
    }    
}

void UMainMenu::OpenJoinMenu()
{
    if (MenuSwitcher)
    {
        if (JoinMenu)
        {
            MenuSwitcher->SetActiveWidget(JoinMenu);
            if (MenuInterface)
            {
                MenuInterface->RefreshServers();
            }
        }
    }
}

void UMainMenu::HostServer()
{
    FString InServerName;
    if (HostNameInput)
    {
        InServerName = HostNameInput->GetText().ToString();
    }
    if (MenuInterface)
    {
        MenuInterface->Host(InServerName);
    }
}

void UMainMenu::CreateServerList(TArray<FServerData> ServerListData)
{
    UWorld* World = GetWorld();

    if (World && ServerList && ServerRowClass)
    {
        ServerList->ClearChildren();
        uint32 i = 0;

        for (const FServerData ServerIterator : ServerListData)
        {
            ServerRow = CreateWidget<UServerRow>(GetWorld(), ServerRowClass);

            if (ServerRow)
            {
                if (ServerRow->ServerName)
                {
                    ServerRow->ServerName->SetText(FText::FromString(ServerIterator.Name));
                }

                if (ServerRow->HostUserName)
                {
                    ServerRow->HostUserName->SetText(FText::FromString(ServerIterator.HostUserName));
                }

                if (ServerRow->PlayerCount)
                {
                    FString CurrentPlayers;
                    CurrentPlayers.AppendInt(ServerIterator.CurrentPlayers);
                    CurrentPlayers.Append("/");
                    CurrentPlayers.AppendInt(ServerIterator.MaxPlayers);
                    ServerRow->PlayerCount->SetText(FText::FromString(CurrentPlayers));
                }
                ServerRow->Setup(this, i);
                ++i;
            }
            
            ServerList->AddChild(ServerRow);
        }
        // FString TestServerName = "";
        // for (int i = 0; i < 3; i++)
        // {
        //     ServerRow = CreateWidget<UServerRow>(GetWorld(), ServerRowClass);
        //     if (ServerRow && ServerRow->ServerName)
        //     {
        //         TestServerName = "Test Server ";
        //         TestServerName.AppendInt(i);
        //         ServerRow->ServerName->SetText(FText::FromString(TestServerName));
        //         ServerRow->Setup(this, i);
        //     }
        //     ServerList->AddChild(ServerRow);
        // }
    }
}

void UMainMenu::RefreshServerList()
{
    if (MenuInterface)
    {
        MenuInterface->RefreshServers();
    }    
}

void UMainMenu::SelectIndex(uint32 Index)
{
    SelectedIndex = Index;
    UE_LOG(LogTemp, Warning, TEXT("Selected index: %d."), SelectedIndex.GetValue());
    UpdateChildren();
}

void UMainMenu::UpdateChildren()
{
    if (ServerList)
    {
        for (int i = 0; i < ServerList->GetChildrenCount(); i++)
        {
            UServerRow* ChildRow = Cast<UServerRow>(ServerList->GetChildAt(i));

            if (SelectedIndex.IsSet() && ChildRow->GetRowIndex() == SelectedIndex.GetValue())
            {
                ChildRow->ColorRowAsSelected();
            }
            else
            {
                ChildRow->ColorRowAsNotSelected();
            }
        }
    }
}

void UMainMenu::JoinServer()
{
    if (SelectedIndex.IsSet() && MenuInterface)
    {
        MenuInterface->Join(SelectedIndex.GetValue());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected index not set."));
    }

}

void UMainMenu::UpdateSessionInfo(FServerData Data)
{
    UE_LOG(LogTemp, Warning, TEXT("Broadcast received.  Updating session."))
    if (ServerList)
    {
        for (int i = 0; i < ServerList->GetChildrenCount(); i++)
        {
            UServerRow* ChildRow = Cast<UServerRow>(ServerList->GetChildAt(i));

            if (ChildRow && ChildRow->ServerName && ChildRow->PlayerCount)
            {
                FString Server = ChildRow->ServerName->GetText().ToString();
                if (Server.Equals(Data.Name, ESearchCase::IgnoreCase))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Found a server to update."));
                    FString CurrentPlayers;
                    CurrentPlayers.AppendInt(Data.CurrentPlayers);
                    CurrentPlayers.Append("/");
                    CurrentPlayers.AppendInt(Data.MaxPlayers);
                    ChildRow->PlayerCount->SetText(FText::FromString(CurrentPlayers));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Child row, server name, or player count does not exist."));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Server list does not exist"));
    }
}

void UMainMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
    RemoveFromViewport();

    if (InWorld)
    {
        APlayerController* Controller = InWorld->GetFirstPlayerController();
        if (Controller)
        {
            FInputModeGameOnly InputModeData;
            Controller->SetInputMode(InputModeData);
            Controller->bShowMouseCursor = false;
            Super::OnLevelRemovedFromWorld(InLevel, InWorld);
        }
        ALobbyGameState* GameState = Cast<ALobbyGameState>(InWorld->GetGameState());

        if (GameState)
        {
            GameState->OnServerDataChange.RemoveDynamic(this, &UMainMenu::UpdateSessionInfo);
        }
    }
}

void UMainMenu::ExitGame()
{
    if (MenuInterface)
    {
        MenuInterface->Exit();
    }
}
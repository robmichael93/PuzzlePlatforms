// Fill out your copyright notice in the Description page of Project Settings.


#include "PopupMenu.h"
#include "Components/Button.h"


void UPopupMenu::SetPopupMenuInterface(IPopupMenuInterface* NewPopupMenuInterface)
{
    PopupMenuInterface = NewPopupMenuInterface;
}

bool UPopupMenu::Initialize()
{
    bool Success = Super::Initialize();
    if (Success)
    {
        if (PopupMenuCancelButton)
        {
            PopupMenuCancelButton->OnClicked.AddDynamic(this, &UPopupMenu::CancelPopupMenu);
        }
        else
        {
            return false;
        }
        if (PopupMenuQuitButton)
        {
            PopupMenuQuitButton->OnClicked.AddDynamic(this, &UPopupMenu::Quit);
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

void UPopupMenu::CancelPopupMenu()
{
    if (PopupMenuInterface)
    {
        PopupMenuInterface->PopupMenuToggle();
    }
}

void UPopupMenu::Quit()
{
    if (PopupMenuInterface)
    {
        PopupMenuInterface->QuitGame();
    }    
}
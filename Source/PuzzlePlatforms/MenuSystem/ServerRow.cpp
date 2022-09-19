// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"
#include "Components/Button.h"
#include "MainMenu.h"

void UServerRow::Setup(class UMainMenu* Parent, uint32 Index)
{
    if (Parent)
    {
        MainMenu = Parent;
    }
    if (ServerSelectButton)
    {
        ServerSelectButton->OnClicked.AddDynamic(this, &UServerRow::SelectSession);
    }
    RowIndex = Index;
}

void UServerRow::SelectSession()
{
    if (MainMenu)
    {
        MainMenu->SelectIndex(RowIndex);
    }
}
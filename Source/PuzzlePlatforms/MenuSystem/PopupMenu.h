// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupMenuInterface.h"
#include "PopupMenu.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UPopupMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPopupMenuInterface(IPopupMenuInterface* NewMenuInterface);

	virtual bool Initialize();
protected:

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* PopupMenuCancelButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* PopupMenuQuitButton;

	UFUNCTION()
	void CancelPopupMenu();

	UFUNCTION()
	void Quit();

	IPopupMenuInterface* PopupMenuInterface;
};

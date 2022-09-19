// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PopupMenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPopupMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PUZZLEPLATFORMS_API IPopupMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void PopupMenuToggle() = 0;
	virtual void QuitGame() = 0;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UHorizontalBox* ServerProperties;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ServerName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* HostUserName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* PlayerCount;

	void Setup(class UMainMenu* Parent, uint32 Index);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetRowIndex() { return RowIndex; }

	UFUNCTION(BlueprintImplementableEvent)
	void ColorRowAsSelected();

	UFUNCTION(BlueprintImplementableEvent)
	void ColorRowAsNotSelected();

private:
	uint32 RowIndex = 0;

	UPROPERTY()
	class UMainMenu* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* ServerSelectButton;
	
	UFUNCTION()
	void SelectSession();
};

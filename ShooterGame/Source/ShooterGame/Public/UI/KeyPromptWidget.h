// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeyPromptWidget.generated.h"

class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UKeyPromptWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(meta = (BindWidget))
	UImage* KeyPromptImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KeyPromptText;
};

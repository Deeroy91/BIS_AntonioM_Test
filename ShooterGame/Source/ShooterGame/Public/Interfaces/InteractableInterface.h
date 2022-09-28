// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

class AActor;
class USoundCue;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHOOTERGAME_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual USoundCue* GetInteractSound();
	virtual FTransform GetInteractableTransform();
	virtual bool OnInteract(AActor* const Instigator);
	virtual void StartFocus();
	virtual void EndFocus();
};

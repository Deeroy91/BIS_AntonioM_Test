// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Interfaces/InteractableInterface.h"

USoundCue* IInteractableInterface::GetInteractSound()
{
	return nullptr;
}

FTransform IInteractableInterface::GetInteractableTransform()
{
	return FTransform();
}

bool IInteractableInterface::OnInteract(AActor* const Instigator)
{
	return false;
}

void IInteractableInterface::StartFocus()
{

}

void IInteractableInterface::EndFocus()
{

}

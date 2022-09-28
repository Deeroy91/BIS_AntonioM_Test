// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "InteractionComponent.generated.h"

USTRUCT()
struct FInteractableItemState
{
	GENERATED_BODY()

	UPROPERTY()
	USoundCue* InteractSound;

	UPROPERTY()
	FTransform InteractableTransform;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

	/** Get interactable the play is lookig at */
	IInteractableInterface* GetInteractableInView();

	void Interact();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerInteract();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_LastInteractedItemState();

protected:

	/* Max distance to focus on interactables */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float MaxUseDistance = 500.0f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastInteractedItemState)
	FInteractableItemState LastInteractedItemState;

private: 

	/* Interactable in center-view. */
	IInteractableInterface* FocusedInteractable;

	FTimerHandle InteractTimerHandle;

	bool bHasNewFocus;
};

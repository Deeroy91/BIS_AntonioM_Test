// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Player/InteractionComponent.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInteractionComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractionComponent, LastInteractedItemState);
}

// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const AShooterCharacter* const OwnerCharacter = Cast<AShooterCharacter>(GetOwner());

	if (OwnerCharacter != nullptr && OwnerCharacter->IsLocallyControlled())
	{
		IInteractableInterface* Interactable = GetInteractableInView();

		if (FocusedInteractable != Interactable)
		{
			if (FocusedInteractable)
			{
				FocusedInteractable->EndFocus();
			}

			bHasNewFocus = true;
		}

		FocusedInteractable = Interactable;

		if (Interactable != nullptr)
		{
			if (bHasNewFocus)
			{
				Interactable->StartFocus();
				bHasNewFocus = false;
			}
		}
	}
}

IInteractableInterface* UInteractionComponent::GetInteractableInView()
{
	UWorld* const World = GetWorld();
	if(World == nullptr)
	{
		return nullptr;
	}

	const AShooterCharacter* const OwnerCharacter = Cast<AShooterCharacter>(GetOwner());

	if(OwnerCharacter == nullptr)
	{
		return nullptr;
	}

	FVector EyeLocation;
	FRotator EyeRotation;

	OwnerCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	const FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * MaxUseDistance);

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(OwnerCharacter);
	TraceParams.AddIgnoredActor(OwnerCharacter->GetWeapon());
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	FHitResult Hit;
	World->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_PROJECTILE, TraceParams);

	return Cast<IInteractableInterface>(Hit.GetActor());
}

void UInteractionComponent::Interact()
{
	IInteractableInterface* Interactable = GetInteractableInView();
	if (Interactable == nullptr)
	{
		return;
	}

	const FTransform InteractableTransform = Interactable->GetInteractableTransform();
	USoundCue* const InteractSound = Interactable->GetInteractSound();

	if(Interactable->OnInteract(GetOwner()))
	{
		LastInteractedItemState.InteractableTransform = InteractableTransform;
		LastInteractedItemState.InteractSound = InteractSound;
	}
}

void UInteractionComponent::ServerInteract_Implementation()
{
	Interact();
}

bool UInteractionComponent::ServerInteract_Validate()
{
	return true;
}

void UInteractionComponent::OnRep_LastInteractedItemState()
{
	UGameplayStatics::PlaySoundAtLocation(this, LastInteractedItemState.InteractSound, LastInteractedItemState.InteractableTransform.GetLocation());
}


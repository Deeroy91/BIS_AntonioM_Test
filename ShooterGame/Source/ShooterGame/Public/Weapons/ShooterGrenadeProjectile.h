// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ShooterProjectile.h"
#include "Interfaces/InteractableInterface.h"
#include "ShooterGrenadeProjectile.generated.h"

class UPrimitiveComponent;
class UWidgetComponent;

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterGrenadeProjectile : public AShooterProjectile, public IInteractableInterface
{
	GENERATED_BODY()
	
public:

	AShooterGrenadeProjectile(const FObjectInitializer& ObjectInitializer);

	/** initial setup */
	virtual void PostInitializeComponents() override;

	/** setup velocity */
	virtual void InitVelocity(const FVector& ShootDirection, const FVector& EndLocation) override;

	virtual void SetHandleImpact() override;

	virtual USoundCue* GetInteractSound() override;

	virtual FTransform GetInteractableTransform() override; 

	virtual bool OnInteract(AActor* const Instigator) override;

	virtual void StartFocus() override;

	virtual void EndFocus() override;

protected:

	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	/** handle hit */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void UnAttachFromCharacter(AShooterCharacter* const ShooterCharacter);

	UFUNCTION()
	void TriggerExplosion();

private:

	void UpdateRotationBasedOnPlayer();

protected:

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	UWidgetComponent* InteractPrompt;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundCue* PickupSound;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundCue* ImpactSound;

private:

	FTimerHandle ExplosionTimerHandle;
	FTimerHandle UpdateWidgetTimerHandle;
};

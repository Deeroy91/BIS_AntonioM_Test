// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ShooterProjectile.h"
#include "ShooterRocketProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterRocketProjectile : public AShooterProjectile
{
	GENERATED_BODY()

public:

	/** initial setup */
	virtual void PostInitializeComponents() override;

	/** set handle hit behaviour */
	virtual void SetHandleImpact() override;

	/** handle hit */
	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);
};

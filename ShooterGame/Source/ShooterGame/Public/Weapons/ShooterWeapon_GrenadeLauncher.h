// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ShooterWeapon_Projectile.h"
#include "ShooterWeapon_GrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterWeapon_GrenadeLauncher : public AShooterWeapon_Projectile
{
	GENERATED_BODY()

protected:

	virtual EAmmoType GetAmmoType() const override
	{
		return EAmmoType::EGrenade;
	}

	virtual void ActivateAlternativeTargeting();

	virtual void DeactivateAlternativeTargeting();

	UFUNCTION()
	void UpdateProjectilePath();

	FPredictProjectilePathResult CalculateProjectilePath(FPredictProjectilePathParams& ProjectilePathParams, const FVector StartLocation, const FVector TossVelocity);
	
	void DrawProjectilePath(const FPredictProjectilePathResult& ProjectilePathResult, int32 StartIndex);

	void ClearProjectilePath();

private:

	FTimerHandle PathTimerHandle;
};

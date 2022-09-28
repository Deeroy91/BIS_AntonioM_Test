// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ShooterWeapon_Projectile.h"
#include "ShooterWeapon_RocketLauncher.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterWeapon_RocketLauncher : public AShooterWeapon_Projectile
{
	GENERATED_BODY()

protected:

	virtual EAmmoType GetAmmoType() const override
	{
		return EAmmoType::ERocket;
	}
	
};

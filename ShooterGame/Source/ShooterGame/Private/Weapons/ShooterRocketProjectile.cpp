// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Weapons/ShooterRocketProjectile.h"

void AShooterRocketProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetLifeSpan(WeaponConfig.ProjectileLife);
}

void AShooterRocketProjectile::SetHandleImpact()
{
	if (MovementComp != nullptr)
	{
		MovementComp->OnProjectileStop.AddDynamic(this, &AShooterRocketProjectile::OnImpact);
	}
}

void AShooterRocketProjectile::OnImpact(const FHitResult& HitResult)
{
	if (GetLocalRole() == ROLE_Authority && !bExploded)
	{
		Explode(HitResult);
		DisableAndDestroy();
	}
}

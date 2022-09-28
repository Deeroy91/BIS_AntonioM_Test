// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ShooterWeapon.h"
#include "GameFramework/DamageType.h" // for UDamageType::StaticClass()
#include "ShooterWeapon_Projectile.generated.h"

// AntonioM BEGIN
class USplineComponent;
class UDecalComponent;
class UStaticMesh;
class USplineMeshComponent;
// AntonioM END

namespace ProjectileVariables
{
constexpr float ProjectileAdjustRange = 20000.0f;
constexpr float ProjectileArcParam = 0.9f;
}

USTRUCT()
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** projectile class */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AShooterProjectile> ProjectileClass;

	/** life time */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float ProjectileLife;

	/** damage at impact point */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	int32 ExplosionDamage;

	/** radius of damage */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float ExplosionRadius;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	TSubclassOf<UDamageType> DamageType;

	/** defaults */
	FProjectileWeaponData()
	{
		ProjectileClass = NULL;
		ProjectileLife = 10.0f;
		ExplosionDamage = 100;
		ExplosionRadius = 300.0f;
		DamageType = UDamageType::StaticClass();
	}
};

// A weapon that fires a visible projectile
UCLASS(Abstract)
class AShooterWeapon_Projectile : public AShooterWeapon
{
	GENERATED_UCLASS_BODY()

public:

	/** perform initial setup */
	virtual void PostInitializeComponents() override;

	/** apply config on projectile */
	void ApplyWeaponConfig(FProjectileWeaponData& Data);

protected:

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() override;

	/** spawn projectile on server */
	UFUNCTION(reliable, server, WithValidation)
	void ServerFireProjectile(FVector Origin, FVector EndLocation, FVector_NetQuantizeNormal ShootDir);

protected:

	// AntonioM BEGIN
	UPROPERTY(EditDefaultsOnly, Category="ProjectilePath")
	USplineComponent* ProjectilePath;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectilePath")
	UDecalComponent* ImpactMarker;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectilePath")
	UStaticMesh* ProjectilePathMesh;

	UPROPERTY()
	TArray<USplineMeshComponent*> SplinePoints;
	// AntonioM END

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category=Config)
	FProjectileWeaponData ProjectileConfig;
};

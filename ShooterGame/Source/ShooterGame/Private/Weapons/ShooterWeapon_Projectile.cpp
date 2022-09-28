// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "Weapons/ShooterWeapon_Projectile.h"
#include "Weapons/ShooterProjectile.h"
// AntonioM BEGIN
#include "Components/SplineComponent.h"
#include "Components/DecalComponent.h"
// AntonioM END

AShooterWeapon_Projectile::AShooterWeapon_Projectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// AntonioM BEGIN
	ProjectilePath = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("ProjectilePath"));
	ProjectilePath->SetupAttachment(GetRootComponent());
	ProjectilePath->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ImpactMarker = ObjectInitializer.CreateDefaultSubobject<UDecalComponent>(this, TEXT("ImpactMarker"));
	ImpactMarker->SetupAttachment(GetRootComponent());
	ImpactMarker->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	ImpactMarker->SetHiddenInGame(true);
	// AntonioM END
}

void AShooterWeapon_Projectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ProjectilePath->AttachTo(GetRootComponent(), MuzzleAttachPoint);
	ProjectilePath->SetWorldLocation(GetMuzzleLocation());
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AShooterWeapon_Projectile::FireWeapon()
{
	FVector ShootDir = GetAdjustedAim();
	FVector Origin = GetMuzzleLocation();

	// trace from camera to check what's under crosshair
	const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * ProjectileVariables::ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	
	// and adjust directions to hit that actor
	if (Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
		bool bWeaponPenetration = false;

		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
		if (DirectionDot < 0.0f)
		{
			// shooting backwards = weapon is penetrating
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			// check for weapon penetration if angle difference is big enough
			// raycast along weapon mesh to check if there's blocking hit

			FVector MuzzleStartTrace = Origin - GetMuzzleDirection() * 150.0f;
			FVector MuzzleEndTrace = Origin;
			FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace);

			if (MuzzleImpact.bBlockingHit)
			{
				bWeaponPenetration = true;
			}
		}

		if (bWeaponPenetration)
		{
			// spawn at crosshair position
			Origin = Impact.ImpactPoint - ShootDir * 10.0f;
		}
		else
		{
			// adjust direction to hit
			ShootDir = AdjustedDir;
		}
	}

	// AntonioM BEGIN
	const FVector EndLocation = Impact.bBlockingHit ? Impact.ImpactPoint : EndTrace;
	// AntonioM END

	ServerFireProjectile(Origin, EndLocation, ShootDir);
}

bool AShooterWeapon_Projectile::ServerFireProjectile_Validate(FVector Origin, FVector EndLocation, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}

void AShooterWeapon_Projectile::ServerFireProjectile_Implementation(FVector Origin, FVector EndLocation, FVector_NetQuantizeNormal ShootDir)
{
	FTransform SpawnTM(ShootDir.Rotation(), Origin);
	AShooterProjectile* Projectile = Cast<AShooterProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));
	if (Projectile)
	{
		Projectile->SetInstigator(GetInstigator());
		Projectile->SetOwner(this);
		Projectile->InitVelocity(ShootDir, EndLocation);

		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
	}
}

void AShooterWeapon_Projectile::ApplyWeaponConfig(FProjectileWeaponData& Data)
{
	Data = ProjectileConfig;
}

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Weapons/ShooterWeapon_Projectile.h"
#include "ShooterProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
// AntonioM BEGIN
class UStaticMeshCompoenent;
// AntonioM END

// 
UCLASS(Abstract, Blueprintable)
class AShooterProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

	/** initial setup */
	virtual void PostInitializeComponents() override;

	/** setup velocity */
	virtual void InitVelocity(const FVector& ShootDirection, const FVector& EndLocation);

	// AntonioM BEGIN
	/** set handle hit behaviour */
	virtual void SetHandleImpact();
	// AntonioM END

protected:

	/** effects for explosion */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<class AShooterExplosionEffect> ExplosionTemplate;

	/** movement component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UProjectileMovementComponent* MovementComp;

	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UParticleSystemComponent* ParticleComp;

	// AntonioM BEGIN
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* StaticMeshComp;
	// AntonioM END

	/** controller that fired me (cache for damage calculations) */
	TWeakObjectPtr<AController> MyController;

	/** projectile data */
	struct FProjectileWeaponData WeaponConfig;

	/** did it explode? */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_Exploded)
	bool bExploded;

	/** [client] explosion happened */
	UFUNCTION()
	void OnRep_Exploded();

	/** trigger explosion */
	virtual void Explode(const FHitResult& Impact);

	/** shutdown projectile and prepare for destruction */
	void DisableAndDestroy();

	/** update velocity on client */
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

protected:

	/** Returns MovementComp subobject **/
	FORCEINLINE UProjectileMovementComponent* GetMovementComp() const { return MovementComp; }

	/** Returns CollisionComp subobject **/
	FORCEINLINE USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ParticleComp subobject **/
	FORCEINLINE UParticleSystemComponent* GetParticleComp() const { return ParticleComp; }

	// AntonioM BEGIN
	/** Returns StaticMeshComp subobject **/
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComp() const { return StaticMeshComp; }
	// AntonioM END
};

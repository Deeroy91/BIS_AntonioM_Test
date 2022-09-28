// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "ShooterExplosionEffect.h"

AShooterExplosionEffect::AShooterExplosionEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ExplosionLightComponentName = TEXT("ExplosionLight");

	ExplosionLight = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, ExplosionLightComponentName);
	RootComponent = ExplosionLight;
	ExplosionLight->AttenuationRadius = 400.0;
	ExplosionLight->Intensity = 500.0f;
	ExplosionLight->bUseInverseSquaredFalloff = false;
	ExplosionLight->LightColor = FColor(255, 185, 35);
	ExplosionLight->CastShadows = false;
	ExplosionLight->SetVisibleFlag(true);

	ExplosionLightFadeOut = 0.2f;

	PrimaryActorTick.bCanEverTick = true;

	// AntonioM BEGIN
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	// AntonioM END
}

void AShooterExplosionEffect::BeginPlay()
{
	Super::BeginPlay();

	// AntonioM - FX should not be spawned/played on the server, except listen server
	if(!UKismetSystemLibrary::IsDedicatedServer(this))
	{
		if (ExplosionFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, GetActorLocation(), GetActorRotation());
		}

		if (ExplosionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
		}

		if (Decal.DecalMaterial)
		{
			FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
			RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

			UGameplayStatics::SpawnDecalAttached(Decal.DecalMaterial, FVector(Decal.DecalSize, Decal.DecalSize, 1.0f),
				SurfaceHit.Component.Get(), SurfaceHit.BoneName,
				SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
				Decal.LifeSpan);
		}
	}
}

void AShooterExplosionEffect::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// AntonioM - server should be responsible for destroying this actor
	if(GetLocalRole() == ROLE_Authority)
	{
		const float TimeAlive = GetWorld()->GetTimeSeconds() - CreationTime;
		const float TimeRemaining = FMath::Max(0.0f, ExplosionLightFadeOut - TimeAlive);

		if (TimeRemaining > 0)
		{
			const float FadeAlpha = 1.0f - FMath::Square(TimeRemaining / ExplosionLightFadeOut);

			UPointLightComponent* DefLight = Cast<UPointLightComponent>(GetClass()->GetDefaultSubobjectByName(ExplosionLightComponentName));
			ExplosionLight->SetIntensity(DefLight->Intensity * FadeAlpha);
		}
		else
		{
			Destroy();
		}
	}
}

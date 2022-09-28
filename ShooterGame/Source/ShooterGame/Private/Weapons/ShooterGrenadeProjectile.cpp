// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Weapons/ShooterGrenadeProjectile.h"
#include "Weapons/ShooterWeapon_GrenadeLauncher.h"
#include "Player/ShooterCharacter.h"
#include "Engine/EngineTypes.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"
#include "CollisionQueryParams.h"

AShooterGrenadeProjectile::AShooterGrenadeProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MovementComp->bShouldBounce = true;
	MovementComp->ProjectileGravityScale = 1.0f;
	MovementComp->InitialSpeed = 3000.0f;
	MovementComp->MaxSpeed = 3000.0f;

	InteractPrompt = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("InteractPrompt"));
	InteractPrompt->SetupAttachment(GetRootComponent());
	InteractPrompt->SetVisibility(false);
}

void AShooterGrenadeProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* const World = GetWorld();

	if(World == nullptr)
	{
		return;
	}

	World->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AShooterGrenadeProjectile::TriggerExplosion, WeaponConfig.ProjectileLife, false);
}

void AShooterGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

}

void AShooterGrenadeProjectile::Destroyed()
{
	UWorld* const World = GetWorld();
	if(World != nullptr)
	{
		World->GetTimerManager().ClearTimer(UpdateWidgetTimerHandle);
		World->GetTimerManager().ClearTimer(ExplosionTimerHandle);
	}

	Super::Destroyed();
}

void AShooterGrenadeProjectile::InitVelocity(const FVector& ShootDirection, const FVector& EndLocation)
{
	if (MovementComp != nullptr)
	{
		FVector TossVelocity;

		UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, TossVelocity, GetActorLocation(), EndLocation, 0.0f, ProjectileVariables::ProjectileArcParam);

		MovementComp->Velocity = TossVelocity;
	}
}

void AShooterGrenadeProjectile::SetHandleImpact()
{
	if (CollisionComp != nullptr)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &AShooterGrenadeProjectile::OnHit);
	}
}

void AShooterGrenadeProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!UKismetSystemLibrary::IsDedicatedServer(this) && !MovementComp->IsVelocityUnderSimulationThreshold())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.ImpactPoint);
	}

	AShooterCharacter* const HitShootCharacter = Cast<AShooterCharacter>(OtherActor);
	if (HitShootCharacter == nullptr)
	{
		return;
	}

	if (CollisionComp != nullptr)
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (MovementComp != nullptr)
	{
		MovementComp->bShouldBounce = false;
	}

	if(!HitShootCharacter->IsPendingKill())
	{
		AttachToComponent(HitShootCharacter->GetPawnMesh(), FAttachmentTransformRules::KeepWorldTransform);

		HitShootCharacter->OnCharacterDeath.AddDynamic(this, &AShooterGrenadeProjectile::UnAttachFromCharacter);
	}
}

void AShooterGrenadeProjectile::UnAttachFromCharacter(AShooterCharacter* const ShooterCharacter)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (CollisionComp != nullptr)
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionComp->SetSimulatePhysics(true);
	}

	if (MovementComp != nullptr)
	{
		MovementComp->bShouldBounce = true;
	}

	ShooterCharacter->OnCharacterDeath.RemoveAll(this);
}

void AShooterGrenadeProjectile::TriggerExplosion()
{
	if (GetLocalRole() == ROLE_Authority && !bExploded)
	{
		FVector ProjDirection = GetActorForwardVector();

		FHitResult Impact;
		Impact.ImpactPoint = GetActorLocation();
		Impact.ImpactNormal = -ProjDirection;

		Explode(Impact);
		DisableAndDestroy();
	}
}

USoundCue* AShooterGrenadeProjectile::GetInteractSound()
{
	return PickupSound;
}

FTransform AShooterGrenadeProjectile::GetInteractableTransform()
{
	return GetActorTransform();
}

bool AShooterGrenadeProjectile::OnInteract(AActor* const Actor)
{
	AShooterCharacter* const ShooterCharacter = Cast<AShooterCharacter>(Actor);
	AShooterWeapon* Weapon = (ShooterCharacter ? ShooterCharacter->FindWeapon(AShooterWeapon_GrenadeLauncher::StaticClass()) : nullptr);

	if (Weapon != nullptr)
	{
		const int32 MissingAmmo = FMath::Max(0, Weapon->GetMaxAmmo() - Weapon->GetCurrentAmmo());
		if(MissingAmmo >= 1)
		{
			Weapon->GiveAmmo(1);
			DisableAndDestroy();
			return true;
		}
	}

	return false;
}

void AShooterGrenadeProjectile::StartFocus()
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (InteractPrompt == nullptr)
	{
		return;
	}

	UpdateRotationBasedOnPlayer();

	InteractPrompt->SetVisibility(true);

	if(StaticMeshComp == nullptr)
	{
		return;
	}

	StaticMeshComp->SetRenderCustomDepth(true);	

	World->GetTimerManager().SetTimer(UpdateWidgetTimerHandle, this, &AShooterGrenadeProjectile::UpdateRotationBasedOnPlayer, 0.2f, true);
}

void AShooterGrenadeProjectile::EndFocus()
{
	UWorld* const World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	if (InteractPrompt == nullptr)
	{
		return;
	}

	InteractPrompt->SetVisibility(false);

	if (StaticMeshComp == nullptr)
	{
		return;
	}

	StaticMeshComp->SetRenderCustomDepth(false);

	World->GetTimerManager().ClearTimer(UpdateWidgetTimerHandle);
}

void AShooterGrenadeProjectile::UpdateRotationBasedOnPlayer()
{
	if(InteractPrompt == nullptr)
	{
		return;
	}

	const APlayerCameraManager* const PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if(PlayerCameraManager == nullptr)
	{
		return;
	}

	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerCameraManager->GetCameraLocation());
	
	InteractPrompt->SetWorldRotation(LookAtRotation);
}

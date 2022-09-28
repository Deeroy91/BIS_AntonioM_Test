// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterWeapon_GrenadeLauncher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SplineComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SplineMeshComponent.h"

namespace TraceDefaultValues
{
constexpr float ProjectileDefaultRadius = 20.0f;
constexpr float ProjectileDefaultLaunchSpeed = 2000.0f;
constexpr float SimFrequency = 15.0f;
constexpr float MaxSimTime = 2.0f;
const FVector2D ProjectilePathScale = FVector2D(0.1f, 0.1f);
}

void AShooterWeapon_GrenadeLauncher::ActivateAlternativeTargeting()
{
	const ACharacter* const OwnerCharacter = Cast<ACharacter>(GetOwner());
	if(UKismetSystemLibrary::IsDedicatedServer(this) || (OwnerCharacter != nullptr && !OwnerCharacter->IsLocallyControlled()))
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		World->GetTimerManager().SetTimer(PathTimerHandle, this, &AShooterWeapon_GrenadeLauncher::UpdateProjectilePath, 0.15f, true);
	}
}

void AShooterWeapon_GrenadeLauncher::DeactivateAlternativeTargeting()
{
	const ACharacter* const OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (UKismetSystemLibrary::IsDedicatedServer(this) || (OwnerCharacter != nullptr && !OwnerCharacter->IsLocallyControlled()))
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(PathTimerHandle);

	ClearProjectilePath();
	ImpactMarker->SetHiddenInGame(true);
}

void AShooterWeapon_GrenadeLauncher::ClearProjectilePath()
{
	for (USplineMeshComponent* Mesh : SplinePoints)
	{
		Mesh->DestroyComponent();
	}
	SplinePoints.Empty();
	ProjectilePath->ClearSplinePoints(true);
}

void AShooterWeapon_GrenadeLauncher::UpdateProjectilePath()
{
	//Clear previous path
	ClearProjectilePath();

	// Calculate Projectile Velocity
	const FVector ShootDir = GetAdjustedAim();
	const FVector Origin = GetMuzzleLocation();

	const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * ProjectileVariables::ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	FVector TossVelocity;

	const FVector EndLocation = Impact.bBlockingHit ? Impact.ImpactPoint : EndTrace;

	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, TossVelocity, Origin, EndLocation, 0.0f, ProjectileVariables::ProjectileArcParam);

	// Find path from weapon until first hit and draw it
	FPredictProjectilePathResult ProjectilePathResult;

	FPredictProjectilePathParams ProjectilePathParams;
	ProjectilePathParams.ActorsToIgnore.Add(this);
	ProjectilePathParams.ActorsToIgnore.Add(GetOwner());

	ProjectilePathResult = CalculateProjectilePath(ProjectilePathParams, Origin, TossVelocity);

	SplinePoints.Reserve(ProjectilePathResult.PathData.Num() * 2);

	DrawProjectilePath(ProjectilePathResult, 0);

	// Check if we hit character
	const AShooterCharacter* const HitCharacter = Cast<AShooterCharacter>(ProjectilePathResult.HitResult.Actor);
	const bool bIsNotHittingCharacter = (ProjectilePathResult.HitResult.bBlockingHit && HitCharacter == nullptr);

	// Find path after first bounce, if we did not hit character and draw it
	FPredictProjectilePathResult BouncePathResult;
	FPredictProjectilePathParams BouncePathParams;
	if(bIsNotHittingCharacter)
	{
		const int32 NumberOfPoints = ProjectilePathResult.PathData.Num();
		const FVector BounceVelocity = ProjectilePathResult.PathData[(NumberOfPoints - 2)].Velocity.MirrorByVector(ProjectilePathResult.HitResult.ImpactNormal);
		const FVector StartLocation = ProjectilePathResult.PathData[(NumberOfPoints - 2)].Location;

		BouncePathResult = CalculateProjectilePath(BouncePathParams, StartLocation, BounceVelocity);

		DrawProjectilePath(BouncePathResult, (ProjectilePath->GetNumberOfSplinePoints()));
	}

	// Display impact marker on impact
	FVector DecalLocation;
	FRotator DecalRotation;

	if (bIsNotHittingCharacter)
	{
		DecalLocation = BouncePathResult.HitResult.ImpactPoint;
		DecalRotation = UKismetMathLibrary::MakeRotFromX(BouncePathResult.HitResult.ImpactNormal);
	}
	else
	{
		DecalLocation = ProjectilePathResult.HitResult.ImpactPoint;
		DecalRotation = UKismetMathLibrary::MakeRotFromX(ProjectilePathResult.HitResult.ImpactNormal);
	}

	ImpactMarker->SetWorldLocationAndRotation(DecalLocation, DecalRotation);
	ImpactMarker->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));
	ImpactMarker->SetHiddenInGame(false);
}

FPredictProjectilePathResult AShooterWeapon_GrenadeLauncher::CalculateProjectilePath(FPredictProjectilePathParams& ProjectilePathParams, const FVector StartLocation, const FVector TossVelocity)
{
	FPredictProjectilePathResult ProjectilePathResult;

	ProjectilePathParams.StartLocation = StartLocation;
	ProjectilePathParams.bTraceWithCollision = true;
	ProjectilePathParams.LaunchVelocity = TossVelocity;
	ProjectilePathParams.OverrideGravityZ = 0.0f; // don't override
	ProjectilePathParams.SimFrequency = TraceDefaultValues::SimFrequency;
	ProjectilePathParams.MaxSimTime = TraceDefaultValues::MaxSimTime;
	ProjectilePathParams.ProjectileRadius = TraceDefaultValues::ProjectileDefaultRadius;
	ProjectilePathParams.bTraceComplex = true;

	UGameplayStatics::PredictProjectilePath(this, ProjectilePathParams, ProjectilePathResult);

	return ProjectilePathResult;
}

void AShooterWeapon_GrenadeLauncher::DrawProjectilePath(const FPredictProjectilePathResult& ProjectilePathResult, int32 StartIndex)
{
	int32 Index1 = StartIndex;

	for (const FPredictProjectilePathPointData PathPoint : ProjectilePathResult.PathData)
	{
		ProjectilePath->AddSplinePointAtIndex(PathPoint.Location, Index1, ESplineCoordinateSpace::World, true);
		Index1++;
	}

	for (int32 Index2 = StartIndex; Index2 <= ProjectilePath->GetNumberOfSplinePoints() - 2; Index2++)
	{
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SplineMesh->SetupAttachment(ProjectilePath);
		SplineMesh->bCastDynamicShadow = false;
		SplineMesh->SetForwardAxis(ESplineMeshAxis::Z);
		SplineMesh->SetStaticMesh(ProjectilePathMesh);
		SplineMesh->SetStartScale(TraceDefaultValues::ProjectilePathScale);
		SplineMesh->SetEndScale(TraceDefaultValues::ProjectilePathScale);

		FVector PointLocationStart, PointTangentStart, PointLocationEnd, PointTangentEnd;
		ProjectilePath->GetLocalLocationAndTangentAtSplinePoint((Index2), PointLocationStart, PointTangentStart);
		ProjectilePath->GetLocalLocationAndTangentAtSplinePoint((Index2 + 1), PointLocationEnd, PointTangentEnd);

		SplineMesh->SetStartAndEnd(PointLocationStart, PointTangentStart, PointLocationEnd, PointTangentEnd, true);

		SplineMesh->RegisterComponent();

		SplinePoints.Add(SplineMesh);
	}
}

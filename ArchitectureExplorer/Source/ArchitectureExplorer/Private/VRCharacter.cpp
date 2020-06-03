// Copyright Scott Kolby 2020


#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

#include "DrawDebugHelpers.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VR Root"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Destination Marker"));
	DestinationMarker->SetupAttachment(GetRootComponent());

	PostProcessComponent = CreateDefaultSubobject <UPostProcessComponent>(TEXT("Post Process Component"));
	PostProcessComponent->SetupAttachment(GetRootComponent());

	LeftMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left Motion Controller"));
	LeftMotionController->SetupAttachment(VRRoot);
	LeftMotionController->SetTrackingSource(EControllerHand::Left);

	RightMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right Motion Controller"));
	RightMotionController->SetupAttachment(VRRoot);
	RightMotionController->SetTrackingSource(EControllerHand::Right);

	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportPath"));
	TeleportPath->SetupAttachment(RightMotionController);
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	DestinationMarker->SetVisibility(false);
	CreateBlinkerMaterialInstance();

	//DynamicMesh = NewObject<UStaticMeshComponent>(this);
	//DynamicMesh->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
	//if (TeleportArcMesh)
	//	DynamicMesh->SetStaticMesh(TeleportArcMesh);
	//if (TeleportArcMaterial)
	//	DynamicMesh->SetMaterial(0, TeleportArcMaterial);
	//DynamicMesh->RegisterComponent();  // Important for dynamic components
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SyncActorToPlayspaceMovement();
	UpdateDestinationMarker();
	UpdateBlinkers();
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AVRCharacter::MoveRight);

	PlayerInputComponent->BindAction(TEXT("Teleport"), EInputEvent::IE_Pressed, this, &AVRCharacter::BeginTeleport);
}

void AVRCharacter::CreateBlinkerMaterialInstance()
{
	if (!BlinkerMaterialBase) return;

	BlinkerMaterialInstance = UMaterialInstanceDynamic::Create(BlinkerMaterialBase, this);
	if (!BlinkerMaterialInstance) return;

	PostProcessComponent->AddOrUpdateBlendable(BlinkerMaterialInstance);
}

void AVRCharacter::UpdateBlinkers()
{
	if (!BlinkerRadiusVsVelocity || !BlinkerMaterialInstance) return;

	float Speed = GetVelocity().Size();
	float Radius = BlinkerRadiusVsVelocity->GetFloatValue(Speed);
	BlinkerMaterialInstance->SetScalarParameterValue(TEXT("Radius"), Radius);

	FVector2D Center = GetBlinkerCenter();
	BlinkerMaterialInstance->SetVectorParameterValue(TEXT("Center"), FLinearColor(Center.X, Center.Y, 0.f));
}

void AVRCharacter::DrawTeleportArc(const TArray<FVector>& Path)
{
	UpdateTeleportPath(Path);

	for (USplineMeshComponent* SplineMesh: TeleportArcMeshPool)
	{
		SplineMesh->SetVisibility(false);
	}

	for (int32 i = 0; i < Path.Num() - 1; i++)
	{
		USplineMeshComponent* SplineMesh = nullptr;
		if (TeleportArcMeshPool.Num() <= i)
		{
			SplineMesh = NewObject<USplineMeshComponent>(this);
			SplineMesh->SetMobility(EComponentMobility::Movable);
			SplineMesh->AttachToComponent(TeleportPath, FAttachmentTransformRules::KeepRelativeTransform);
			
			if (TeleportArcMesh)
				SplineMesh->SetStaticMesh(TeleportArcMesh);

			if (TeleportArcMaterial)
				SplineMesh->SetMaterial(0, TeleportArcMaterial);
				
			SplineMesh->RegisterComponent();  // Important for dynamic components

			TeleportArcMeshPool.Add(SplineMesh);
		}

		SplineMesh = TeleportArcMeshPool[i];

		FVector StartLocation, StartTangent, EndLocation, EndTangent;
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent);
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent);

		SplineMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
		SplineMesh->SetVisibility(true);
	}
}

void AVRCharacter::UpdateTeleportPath(const TArray<FVector>& Path)
{
	TeleportPath->ClearSplinePoints(false);
	for (int32 i = 0; i < Path.Num(); i++)
	{
		FVector LocalPosition = TeleportPath->GetComponentTransform().InverseTransformPosition(Path[i]);  // Convert to local coordinates
		FSplinePoint SplinePoint = FSplinePoint(i, LocalPosition, ESplinePointType::Curve);
		TeleportPath->AddPoint(SplinePoint, false);
	}
	TeleportPath->UpdateSpline();
}

FVector2D AVRCharacter::GetBlinkerCenter()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return FVector2D(0.5f);

	FVector MovementDirection = GetVelocity().GetSafeNormal();
	if (MovementDirection.IsNearlyZero()) 
		return FVector2D(0.5f);
	
	FVector WorldStationaryLocation;
	if (FVector::DotProduct(Camera->GetForwardVector(), MovementDirection) > 0)
		WorldStationaryLocation = Camera->GetComponentLocation() + MovementDirection * 100;  // Forward movement
	else
		WorldStationaryLocation = Camera->GetComponentLocation() - MovementDirection * 100;  // Backward movement
	

	FVector2D ScreenStationaryLocation;
	PlayerController->ProjectWorldLocationToScreen(WorldStationaryLocation, ScreenStationaryLocation);

	int32 SizeX, SizeY;
	PlayerController->GetViewportSize(SizeX, SizeY);

	return FVector2D(ScreenStationaryLocation.X / (float)SizeX, ScreenStationaryLocation.Y / (float)SizeY);
}

void AVRCharacter::SyncActorToPlayspaceMovement()
{
	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0.f;  // Only move in X Y.  Use FVector::VectorPlaneProject() if Z is not up
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);
}


void AVRCharacter::UpdateDestinationMarker()
{
	if (!DestinationMarker) return;

	TArray<FVector> Path;
	FVector Location;
	bool bHasDestination = FindTeleportDestination(Path, Location);

	if (bHasDestination)
	{
		DestinationMarker->SetVisibility(true);
		DestinationMarker->SetWorldLocation(Location);
	}
	else
	{
		Path.Empty();
		DestinationMarker->SetVisibility(false);
	}

	DrawTeleportArc(Path);
}

bool AVRCharacter::FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation) const
{
	if (!DestinationMarker || !RightMotionController || !RightMotionController->IsTracked()) return false;
		
	FVector LookVector = RightMotionController->GetForwardVector();
	FVector Start = RightMotionController->GetComponentLocation() + LookVector * 20.f;  // Start line trace in front of motion controller
	FVector LaunchVector = RightMotionController->GetForwardVector() * TeleportProjectileSpeed;

	FPredictProjectilePathParams ProjectileParams(
		TeleportProjectileRadius, 
		Start, 
		LaunchVector, 
		TeleportSimulationTime, 
		ECollisionChannel::ECC_Visibility);
	//ProjectileParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	ProjectileParams.bTraceComplex = true;  // Get more collision locations if needed

	FPredictProjectilePathResult ProjectileResult;

	bool bHit = UGameplayStatics::PredictProjectilePath(this, ProjectileParams, ProjectileResult);

	if (!bHit) return false;

	FNavLocation NavLocation;

	for (FPredictProjectilePathPointData PointData : ProjectileResult.PathData)
	{
		OutPath.Add(PointData.Location);
	}

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	bool bOnNavMesh = NavSystem->ProjectPointToNavigation(ProjectileResult.HitResult.Location, NavLocation, TeleportProjectionExtent);

	if (!bOnNavMesh) return false;

	OutLocation = NavLocation.Location;

	return true;
}

bool AVRCharacter::FindTeleportDestinationHMD(FVector& OutLocation) const
{
	if (!DestinationMarker || !Camera) return false;

	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	float MaxTeleportDistance = 1000.f;
	FVector End = Start + Camera->GetForwardVector() * MaxTeleportDistance;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.0f, 0.0f, 2.0f);

	if (!bHit) return false;

	FNavLocation NavLocation;
	bool bOnNavMesh = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportProjectionExtent);

	if (!bOnNavMesh) return false;

	OutLocation = NavLocation.Location;

	return true;
}

void AVRCharacter::MoveForward(float AxisValue)
{
	if (!Camera) return;
	float MoveValue = FMath::Clamp<float>(AxisValue, -1, 1);
	AddMovementInput(Camera->GetForwardVector() * MoveValue);
}

void AVRCharacter::MoveRight(float AxisValue)
{
	if (!Camera) return;
	float MoveValue = FMath::Clamp<float>(AxisValue, -1, 1);
	AddMovementInput(Camera->GetRightVector() * MoveValue);
}

void AVRCharacter::StartFade(float FromAlpha, float ToAlpha)
{
	auto PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	auto PlayerCameraManager = PlayerController->PlayerCameraManager;
	if (PlayerCameraManager)
	{
		PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, TeleportFadeDuration, FLinearColor::Black);
	}
}

void AVRCharacter::BeginTeleport()
{
	StartFade(0.f, 1.f);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AVRCharacter::EndTeleport, TeleportFadeDuration);
}

void AVRCharacter::EndTeleport()
{
	FVector Destination = DestinationMarker->GetComponentLocation();
	Destination += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * GetActorUpVector();
	SetActorLocation(Destination);

	StartFade(1.0f, 0.f);
}

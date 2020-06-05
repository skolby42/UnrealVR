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
#include "HandController.h"
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
	
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetCollisionProfileName(TEXT("NoCollision"));
	DestinationMarker->SetupAttachment(GetRootComponent());

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(GetRootComponent());

	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportPath"));
	TeleportPath->SetupAttachment(VRRoot);
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(DestinationMarker))
		DestinationMarker->SetVisibility(false);
	CreateBlinkerMaterialInstance();
	CreateHandControllers();
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
	
	PlayerInputComponent->BindAction(TEXT("Teleport"), EInputEvent::IE_Pressed, this, &AVRCharacter::ActivateTeleport);
	PlayerInputComponent->BindAction(TEXT("Teleport"), EInputEvent::IE_Released, this, &AVRCharacter::BeginTeleport);
	PlayerInputComponent->BindAction(TEXT("GripLeft"), EInputEvent::IE_Pressed, this, &AVRCharacter::GripLeft);
	PlayerInputComponent->BindAction(TEXT("GripLeft"), EInputEvent::IE_Released, this, &AVRCharacter::ReleaseLeft);
	PlayerInputComponent->BindAction(TEXT("GripRight"), EInputEvent::IE_Pressed, this, &AVRCharacter::GripRight);
	PlayerInputComponent->BindAction(TEXT("GripRight"), EInputEvent::IE_Released, this, &AVRCharacter::ReleaseRight);

	if (bDoSnapTurn)
	{
		PlayerInputComponent->BindAction(TEXT("SnapTurnLeft"), EInputEvent::IE_Released, this, &AVRCharacter::SnapTurnLeft);
		PlayerInputComponent->BindAction(TEXT("SnapTurnRight"), EInputEvent::IE_Released, this, &AVRCharacter::SnapTurnRight);
	}
	else
	{
		PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &AVRCharacter::TurnRight);
	}
}

void AVRCharacter::CreateHandControllers()
{
	if (!HandControllerClass) return;

	LeftController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (LeftController)
	{
		LeftController->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
		LeftController->SetHand(EControllerHand::Left);
		LeftController->SetOwner(this);  // Fix for 4.22+
	}

	RightController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (RightController)
	{
		RightController->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
		RightController->SetHand(EControllerHand::Right);
		RightController->SetOwner(this);  // Fix for 4.22+
	}

	LeftController->PairController(RightController);
}

void AVRCharacter::CreateBlinkerMaterialInstance()
{
	if (!BlinkerMaterialBase || !PostProcessComponent) return;

	BlinkerMaterialInstance = UMaterialInstanceDynamic::Create(BlinkerMaterialBase, this);
	if (!BlinkerMaterialInstance) return;

	PostProcessComponent->AddOrUpdateBlendable(BlinkerMaterialInstance);
}

void AVRCharacter::UpdateBlinkers()
{
	if (!BlinkerRadiusVsVelocity || !BlinkerMaterialInstance) return;

	float Speed = BlinkerSpeedOverride > 0 ? BlinkerSpeedOverride : GetVelocity().Size();
	float Radius = BlinkerRadiusVsVelocity->GetFloatValue(Speed);
	BlinkerMaterialInstance->SetScalarParameterValue(TEXT("Radius"), Radius);

	FVector2D Center = GetBlinkerCenter();
	BlinkerMaterialInstance->SetVectorParameterValue(TEXT("Center"), FLinearColor(Center.X, Center.Y, 0.f));
}

FVector2D AVRCharacter::GetBlinkerCenter()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return FVector2D(0.5f);

	FVector MovementDirection = GetVelocity().GetSafeNormal();
	if (MovementDirection.IsNearlyZero() || !bBlinkersUseMovementDirection) 
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

	if (!bIsTeleportActive)
	{
		DestinationMarker->SetVisibility(false);
		DrawTeleportArc(Path);
		return;
	}

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
	if (!DestinationMarker || !RightController) return false;
		
	FVector LookVector = RightController->GetActorForwardVector();
	FVector Start = RightController->GetActorLocation() + LookVector * 3.f;  // Start line trace in front of motion controller
	FVector LaunchVector = LookVector * TeleportProjectileSpeed;

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

void AVRCharacter::DrawTeleportArc(const TArray<FVector>& Path)
{
	if (!TeleportPath) return;

	UpdateTeleportPath(Path);

	for (USplineMeshComponent* SplineMesh : TeleportArcMeshPool)
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
	if (!TeleportPath) return;

	TeleportPath->ClearSplinePoints(false);
	for (int32 i = 0; i < Path.Num(); i++)
	{
		FVector LocalPosition = TeleportPath->GetComponentTransform().InverseTransformPosition(Path[i]);  // Convert to local coordinates
		FSplinePoint SplinePoint = FSplinePoint(i, LocalPosition, ESplinePointType::Curve);
		TeleportPath->AddPoint(SplinePoint, false);
	}
	TeleportPath->UpdateSpline();
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

void AVRCharacter::TurnRight(float AxisValue)
{
	float MoveValue = FMath::Clamp<float>(AxisValue, -1, 1);
	if (MoveValue == 0.f)
	{
		BlinkerSpeedOverride = 0.f;
		return;
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	MoveValue *= DeltaTime * TurnDegreesPerSecond;

	AddControllerYawInput(MoveValue);

	if (BlinkerRadiusVsVelocity)
	{
		float MinTime, MaxTime;
		BlinkerRadiusVsVelocity->GetTimeRange(MinTime, MaxTime);
		BlinkerSpeedOverride = MaxTime * FMath::Abs(MoveValue);
	}
}

void AVRCharacter::SnapTurnRight()
{
	if (bIsTeleportActive) return;

	StartFade(0.f, 1.f, SnapTurnFadeDuration);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AVRCharacter::SnapTurn, TurnDegreesPerSecond), SnapTurnFadeDuration, false);
	
}

void AVRCharacter::SnapTurnLeft()
{
	if (bIsTeleportActive) return;

	StartFade(0.f, 1.f, SnapTurnFadeDuration);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AVRCharacter::SnapTurn, -TurnDegreesPerSecond), SnapTurnFadeDuration, false);
}

void AVRCharacter::SnapTurn(float Rotation)
{
	auto PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	PlayerController->AddYawInput(Rotation / PlayerController->InputYawScale);
	StartFade(1.f, 0.f, SnapTurnFadeDuration);
}

void AVRCharacter::StartFade(float FromAlpha, float ToAlpha, float FadeDuration)
{
	auto PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	auto PlayerCameraManager = PlayerController->PlayerCameraManager;
	if (PlayerCameraManager)
	{
		PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, FadeDuration, FLinearColor::Black);
	}
}

void AVRCharacter::ActivateTeleport()
{
	bIsTeleportActive = true;
}

void AVRCharacter::BeginTeleport()
{
	StartFade(0.f, 1.f, TeleportFadeDuration);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AVRCharacter::EndTeleport, TeleportFadeDuration);
}

void AVRCharacter::EndTeleport()
{
	FVector Destination = DestinationMarker->GetComponentLocation();
	Destination += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * GetActorUpVector();
	SetActorLocation(Destination);

	StartFade(1.0f, 0.f, TeleportFadeDuration);

	bIsTeleportActive = false;
}

void AVRCharacter::GripLeft()
{
	if (!LeftController) return;
	LeftController->Grip();
}

void AVRCharacter::ReleaseLeft()
{
	if (!LeftController) return;
	LeftController->Release();
}

void AVRCharacter::GripRight()
{
	if (!RightController) return;
	RightController->Grip();
}

void AVRCharacter::ReleaseRight()
{
	if (!RightController) return;
	RightController->Release();
}
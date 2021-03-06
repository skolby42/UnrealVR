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

	TeleportArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportArrow"));
	TeleportArrow->SetCollisionProfileName(TEXT("NoCollision"));
	TeleportArrow->SetupAttachment(DestinationMarker);

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(GetRootComponent());

	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportPath"));
	TeleportPath->SetupAttachment(VRRoot);
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (DestinationMarker)
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
	PlayerInputComponent->BindAxis(TEXT("TeleportUp"));
	PlayerInputComponent->BindAxis(TEXT("TeleportRight"));
	
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
		LeftController->SetControllerMeshVisibility(false);
		LeftController->SetOwner(this);  // Fix for 4.22+
	}

	RightController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (RightController)
	{
		RightController->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
		RightController->SetHand(EControllerHand::Right);
		RightController->SetControllerMeshVisibility(false);
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

	if (!bTeleportActive)
	{
		ResetTeleport();
		return;
	}

	if (bTeleportLocked) return;

	bHasTeleportDestination = FindTeleportDestination(Path, Location);

	if (bHasTeleportDestination)
	{
		DestinationMarker->SetVisibility(true);
		DestinationMarker->SetWorldLocation(Location);
		TeleportArrow->SetVisibility(true);
	}
	else
	{
		Path.Empty();
		DestinationMarker->SetVisibility(false);
		TeleportArrow->SetVisibility(false);
	}

	DrawTeleportArc(Path);
	UpdateTeleportArrow();
}

void AVRCharacter::ResetTeleport()
{
	SetTeleportEnabled(false);
	DestinationMarker->SetVisibility(false);
	TeleportArrow->SetVisibility(false);
	TArray<FVector> EmptyPath;
	DrawTeleportArc(EmptyPath);
}

bool AVRCharacter::FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation) const
{
	if (!RightController) return false;
	return RightController->FindTeleportDestination(OutPath, OutLocation);
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
	FVector TeleportProjectionExtent(100.f);
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

void AVRCharacter::UpdateTeleportArrow()
{
	if (!bTeleportActive || bTeleportLocked) return;

	TeleportRotation = FRotator(0.f, GetActorRotation().Yaw, 0.f);

	float ArrowUp = FMath::Clamp<float>(GetInputAxisValue(TEXT("TeleportUp")), -1, 1);
	float ArrowRight = FMath::Clamp<float>(GetInputAxisValue(TEXT("TeleportRight")), -1, 1);
	
	if (FMath::Abs(ArrowUp) > RightController->GetThumbDeadZone() || FMath::Abs(ArrowRight) > RightController->GetThumbDeadZone())
		TeleportRotation += FVector(ArrowUp, ArrowRight, 0.f).Rotation();

	TeleportArrow->SetWorldRotation(TeleportRotation);
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
	if (bTeleportActive) return;
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
	if (bTeleportActive) return;
	StartFade(0.f, 1.f, SnapTurnFadeDuration);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AVRCharacter::SnapTurn, TurnDegreesPerSecond), SnapTurnFadeDuration, false);
}

void AVRCharacter::SnapTurnLeft()
{
	if (bTeleportActive) return;
	StartFade(0.f, 1.f, SnapTurnFadeDuration);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AVRCharacter::SnapTurn, -TurnDegreesPerSecond), SnapTurnFadeDuration, false);
}

void AVRCharacter::SnapTurn(float Rotation)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	PlayerController->AddYawInput(Rotation / PlayerController->InputYawScale);
	StartFade(1.f, 0.f, SnapTurnFadeDuration);
}

void AVRCharacter::StartFade(float FromAlpha, float ToAlpha, float FadeDuration)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	auto PlayerCameraManager = PlayerController->PlayerCameraManager;
	if (PlayerCameraManager)
	{
		PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, FadeDuration, FLinearColor::Black);
	}
}

void AVRCharacter::ActivateTeleport()
{
	if (bTeleportActive) return;
	SetTeleportEnabled(true);
	SetTeleportLocked(false);
}

void AVRCharacter::BeginTeleport()
{
	if (!bHasTeleportDestination)
	{
		ResetTeleport();
		return;
	}

	if (bTeleportLocked) return;

	SetTeleportLocked(true);
	StartFade(0.f, 1.f, TeleportFadeDuration);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AVRCharacter::EndTeleport, TeleportFadeDuration);
}

void AVRCharacter::EndTeleport()
{
	FVector Destination = DestinationMarker->GetComponentLocation();
	Destination += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * GetActorUpVector();
	SetActorLocation(Destination);
	SetActorRotation(TeleportRotation);
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
		PlayerController->SetControlRotation(TeleportRotation);

	ResetTeleport();
	StartFade(1.0f, 0.f, TeleportFadeDuration);
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AVRCharacter::SetTeleportEnabled, false), TeleportFadeDuration, false);
}

void AVRCharacter::SetTeleportEnabled(bool bEnabled)
{
	bTeleportActive = bEnabled;
}

void AVRCharacter::SetTeleportLocked(bool bLocked)
{
	bTeleportLocked = bLocked;
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
// Copyright Scott Kolby 2020


#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	VRRoot = CreateDefaultSubobject<USceneComponent>(FName(TEXT("VR Root")));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(FName(TEXT("Camera")));
	Camera->SetupAttachment(VRRoot);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Destination Marker")));
	DestinationMarker->SetupAttachment(GetRootComponent());

	PostProcessComponent = CreateDefaultSubobject <UPostProcessComponent>(FName(TEXT("Post Process Component")));
	PostProcessComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	DestinationMarker->SetVisibility(false);
	CreateBlinkerMaterialInstance();
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SyncActorToPlayspaceMovement();
	UpdateDestinationMarker();
	UpdateBlinkers();
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
	FVector Location;
	bool bHasDestination = FindTeleportDestination(Location);

	if (bHasDestination)
	{
		DestinationMarker->SetVisibility(true);
		DestinationMarker->SetWorldLocation(Location);
	}
	else
	{
		DestinationMarker->SetVisibility(false);
	}
}

bool AVRCharacter::FindTeleportDestination(FVector& OutLocation) const
{
	if (!DestinationMarker) return false;

	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * MaxTeleportDistance;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

	if (!bHit) return false;

	FNavLocation NavLocation;
	bool bOnNavMesh = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportProjectionExtent);

	if (!bOnNavMesh) return false;

	OutLocation = NavLocation.Location;

	return true;
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AVRCharacter::MoveRight);

	PlayerInputComponent->BindAction(TEXT("Teleport"), EInputEvent::IE_Pressed, this, &AVRCharacter::BeginTeleport);
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
	auto CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector NewLocation = DestinationMarker->GetComponentLocation();
	NewLocation.Z += CapsuleHalfHeight;
	SetActorLocation(NewLocation);

	StartFade(1.0f, 0.f);
}

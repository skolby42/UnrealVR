// Copyright Scott Kolby 2020

#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/InputComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "HandController.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

#include "DrawDebugHelpers.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(GetRootComponent());
}

void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	CreateBlinkerMaterialInstance();
	CreateHandControllers();
}

void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SyncActorToPlayspaceMovement();
	UpdateBlinkers();
}

void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TeleportUp"), this, &AVRCharacter::TeleportUp);
	PlayerInputComponent->BindAxis(TEXT("TeleportRight"), this, &AVRCharacter::TeleportRight);
	
	PlayerInputComponent->BindAction(TEXT("TriggerLeft"), EInputEvent::IE_Pressed, this, &AVRCharacter::PressLeftTrigger);
	PlayerInputComponent->BindAction(TEXT("TriggerLeft"), EInputEvent::IE_Released, this, &AVRCharacter::ReleaseLeftTrigger);
	PlayerInputComponent->BindAction(TEXT("TriggerRight"), EInputEvent::IE_Pressed, this, &AVRCharacter::PressRightTrigger);
	PlayerInputComponent->BindAction(TEXT("TriggerRight"), EInputEvent::IE_Released, this, &AVRCharacter::ReleaseRightTrigger);
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

void AVRCharacter::MoveForward(float AxisValue)
{
	if ((LeftController && LeftController->IsTeleportActive()) || (RightController && RightController->IsTeleportActive())) return;
	if (!Camera) return;

	float MoveValue = FMath::Clamp<float>(AxisValue, -1, 1);
	AddMovementInput(Camera->GetForwardVector() * MoveValue);
}

void AVRCharacter::MoveRight(float AxisValue)
{
	if ((LeftController && LeftController->IsTeleportActive()) || (RightController && RightController->IsTeleportActive())) return;
	if (!Camera) return;

	float MoveValue = FMath::Clamp<float>(AxisValue, -1, 1);
	AddMovementInput(Camera->GetRightVector() * MoveValue);
}

void AVRCharacter::TurnRight(float AxisValue)
{
	if ((LeftController && LeftController->IsTeleportActive()) || (RightController && RightController->IsTeleportActive())) return;

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
	if ((LeftController && LeftController->IsTeleportActive()) || (RightController && RightController->IsTeleportActive())) return;

	StartFade(0.f, 1.f, SnapTurnFadeDuration);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AVRCharacter::SnapTurn, TurnDegreesPerSecond), SnapTurnFadeDuration, false);
}

void AVRCharacter::SnapTurnLeft()
{
	if ((LeftController && LeftController->IsTeleportActive()) || (RightController && RightController->IsTeleportActive())) return;

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

void AVRCharacter::GripLeft()
{
	if (!LeftController) return;
	LeftController->GripPress();
}

void AVRCharacter::ReleaseLeft()
{
	if (!LeftController) return;
	LeftController->GripRelease();
}

void AVRCharacter::GripRight()
{
	if (!RightController) return;
	RightController->GripPress();
}

void AVRCharacter::ReleaseRight()
{
	if (!RightController) return;
	RightController->GripRelease();
}

void AVRCharacter::PressLeftTrigger()
{
	if (!LeftController) return;
	LeftController->TriggerPress();
}

void AVRCharacter::ReleaseLeftTrigger()
{
	if (!LeftController) return;
	LeftController->TriggerRelease();
}

void AVRCharacter::PressRightTrigger()
{
	if (!RightController) return;
	RightController->TriggerPress();
}

void AVRCharacter::ReleaseRightTrigger()
{
	if (!RightController) return;
	RightController->TriggerRelease();
}

void AVRCharacter::TeleportRight(float AxisValue)
{
	if (LeftController && LeftController->IsTeleportActive())
	{
		LeftController->SetTeleportRight(AxisValue);
	}

	if (RightController && RightController->IsTeleportActive())
	{
		RightController->SetTeleportRight(AxisValue);
	}
}

void AVRCharacter::TeleportUp(float AxisValue)
{
	if (LeftController && LeftController->IsTeleportActive())
	{
		LeftController->SetTeleportUp(AxisValue);
	}

	if (RightController && RightController->IsTeleportActive())
	{
		RightController->SetTeleportUp(AxisValue);
	}
}

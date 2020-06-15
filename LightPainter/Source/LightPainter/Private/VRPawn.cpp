// Copyright Scott Kolby 2020

#include "VRPawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "HandControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/StereoLayerFunctionLibrary.h"
#include "PaintingGameMode.h"
#include "LightPainter/UI/PaintingPicker/PaintingPicker.h"

AVRPawn::AVRPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	SetRootComponent(VRRoot);

	Camera = CreateDefaultSubobject <UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);
}

void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	
	CreateHandControllers();
}

void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("RightTrigger"), EInputEvent::IE_Pressed, this, &AVRPawn::TriggerPressed);
	PlayerInputComponent->BindAction(TEXT("RightTrigger"), EInputEvent::IE_Released, this, &AVRPawn::TriggerReleased);

	PlayerInputComponent->BindAxis(TEXT("PaginateRight"), this, &AVRPawn::PaginateRight);
}

void AVRPawn::CreateHandControllers()
{
	if (!RightHandControllerClass) return;
	LeftController = GetWorld()->SpawnActor<AHandControllerBase>(LeftHandControllerClass);
	if (LeftController)
	{
		LeftController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		LeftController->SetHand(EControllerHand::Left);
		LeftController->SetOwner(this);  // Fix for 4.22+
	}

	RightController = GetWorld()->SpawnActor<AHandControllerBase>(RightHandControllerClass);
	if (RightController)
	{
		RightController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		RightController->SetHand(EControllerHand::Right);
		RightController->SetOwner(this);  // Fix for 4.22+
	}

	if (LeftController && RightController)
		LeftController->Pair(RightController);
}

void AVRPawn::PaginateRight(float AxisValue)
{
	float NormalizedAxisValue = FMath::Clamp<float>(AxisValue, -1, 1);

	// Latch analogue input
	int32 PaginationOffset = 0;
	PaginationOffset += NormalizedAxisValue > PaginationThumbstickThreshold ? 1 : 0;
	PaginationOffset += NormalizedAxisValue < -PaginationThumbstickThreshold ? -1 : 0;

	if (PaginationOffset != LastPaginationOffset && PaginationOffset != 0)
	{
		UpdateCurrentPage(PaginationOffset);
	}	

	LastPaginationOffset = PaginationOffset;
}

void AVRPawn::TriggerPressed()
{
	if (!RightController) return;
	RightController->TriggerPressed();
}

void AVRPawn::TriggerReleased()
{
	if (!RightController) return;
	RightController->TriggerReleased();
}

void AVRPawn::UpdateCurrentPage(int32 Offset)
{
	for (TActorIterator<APaintingPicker>PaintingPickerIter(GetWorld()); PaintingPickerIter; ++PaintingPickerIter)
	{
		PaintingPickerIter->UpdateCurrentPage(Offset);
	}
}

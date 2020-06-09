// Copyright Scott Kolby 2020

#include "VRPawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "HandController.h"

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
}

void AVRPawn::CreateHandControllers()
{
	if (!HandControllerClass) return;
	LeftController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (LeftController)
	{
		LeftController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		LeftController->SetHand(EControllerHand::Left);
		LeftController->SetOwner(this);  // Fix for 4.22+
	}

	RightController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (RightController)
	{
		RightController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		RightController->SetHand(EControllerHand::Right);
		RightController->SetOwner(this);  // Fix for 4.22+
	}
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

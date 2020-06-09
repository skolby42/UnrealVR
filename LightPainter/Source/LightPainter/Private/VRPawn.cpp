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
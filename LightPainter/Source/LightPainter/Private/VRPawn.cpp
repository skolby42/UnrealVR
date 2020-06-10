// Copyright Scott Kolby 2020

#include "VRPawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "PaintBrushHandController.h"
#include "LightPainter/Saving/LightPainterSaveGame.h"

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
	
	CreatePaintBrushHandControllers();
}

void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("RightTrigger"), EInputEvent::IE_Pressed, this, &AVRPawn::TriggerPressed);
	PlayerInputComponent->BindAction(TEXT("RightTrigger"), EInputEvent::IE_Released, this, &AVRPawn::TriggerReleased);

	PlayerInputComponent->BindAction(TEXT("Save"), EInputEvent::IE_Released, this, &AVRPawn::Save);
	PlayerInputComponent->BindAction(TEXT("Load"), EInputEvent::IE_Released, this, &AVRPawn::Load);
}

void AVRPawn::CreatePaintBrushHandControllers()
{
	if (!PaintBrushHandControllerClass) return;
	LeftController = GetWorld()->SpawnActor<APaintBrushHandController>(PaintBrushHandControllerClass);
	if (LeftController)
	{
		LeftController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		LeftController->SetHand(EControllerHand::Left);
		LeftController->SetOwner(this);  // Fix for 4.22+
	}

	RightController = GetWorld()->SpawnActor<APaintBrushHandController>(PaintBrushHandControllerClass);
	if (RightController)
	{
		RightController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		RightController->SetHand(EControllerHand::Right);
		RightController->SetOwner(this);  // Fix for 4.22+
	}
}

void AVRPawn::Save()
{
	ULightPainterSaveGame* SaveGame = ULightPainterSaveGame::Create();
	if (SaveGame)
	{
		SaveGame->SerializeFromWorld(GetWorld());
		SaveGame->Save();
	}	
}

void AVRPawn::Load()
{
	ULightPainterSaveGame* SaveGame = ULightPainterSaveGame::Load();
	if (SaveGame)
	{
		SaveGame->DeserializeToWorld(GetWorld());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save game not loaded"))
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

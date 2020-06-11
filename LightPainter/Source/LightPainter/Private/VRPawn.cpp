// Copyright Scott Kolby 2020

#include "VRPawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "HandControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/StereoLayerFunctionLibrary.h"
#include "LightPainter/Saving/LightPainterSaveGame.h"
#include "PaintingGameMode.h"

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

	/*ULightPainterSaveGame* SaveGame = ULightPainterSaveGame::Create();
	if (SaveGame)
	{
		CurrentSlotName = SaveGame->GetSlotName();
	}*/
}

void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("RightTrigger"), EInputEvent::IE_Pressed, this, &AVRPawn::TriggerPressed);
	PlayerInputComponent->BindAction(TEXT("RightTrigger"), EInputEvent::IE_Released, this, &AVRPawn::TriggerReleased);

	PlayerInputComponent->BindAction(TEXT("Save"), EInputEvent::IE_Released, this, &AVRPawn::Save);
}

void AVRPawn::CreateHandControllers()
{
	if (!HandControllerClass) return;
	LeftController = GetWorld()->SpawnActor<AHandControllerBase>(HandControllerClass);
	if (LeftController)
	{
		LeftController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		LeftController->SetHand(EControllerHand::Left);
		LeftController->SetOwner(this);  // Fix for 4.22+
	}

	RightController = GetWorld()->SpawnActor<AHandControllerBase>(HandControllerClass);
	if (RightController)
	{
		RightController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		RightController->SetHand(EControllerHand::Right);
		RightController->SetOwner(this);  // Fix for 4.22+
	}
}

void AVRPawn::Save()
{
	auto GameMode = Cast<APaintingGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	GameMode->Save();
}

void AVRPawn::ReturnToMainMenu()
{
	UStereoLayerFunctionLibrary::ShowSplashScreen();  // Workaround for bug causing hang when opening level
	// TODO refactor magic string
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));
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

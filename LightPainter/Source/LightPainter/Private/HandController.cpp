// Copyright Scott Kolby 2020

#include "HandController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "MotionControllerComponent.h"
#include "Stroke.h"

AHandController::AHandController()
{
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);
	//MotionController->SetShowDeviceModel(true);

	ControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControllerMesh"));
	ControllerMesh->SetupAttachment(MotionController);
}

void AHandController::BeginPlay()
{
	Super::BeginPlay();

}

void AHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AHandController::SetHand(EControllerHand ControllerHand)
{
	if (!MotionController) return;
	MotionController->SetTrackingSource(ControllerHand);
	UpdateControllerMesh(ControllerHand);
}

UStaticMeshComponent* AHandController::GetControllerMesh()
{
	return ControllerMesh;
}

void AHandController::TriggerPressed()
{
	if (!StrokeClass) return;
	AStroke* Stroke = GetWorld()->SpawnActor<AStroke>(StrokeClass);
	Stroke->SetActorLocation(GetActorLocation() + GetActorForwardVector() * 5.f);
}

void AHandController::TriggerReleased()
{
}

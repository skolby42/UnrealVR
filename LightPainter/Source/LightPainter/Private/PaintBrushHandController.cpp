// Copyright Scott Kolby 2020

#include "PaintBrushHandController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Stroke.h"

APaintBrushHandController::APaintBrushHandController()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APaintBrushHandController::BeginPlay()
{
	Super::BeginPlay();

}

void APaintBrushHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CurrentStroke)
	{
		CurrentStroke->Update(GetCursorLocation());
	}
}

void APaintBrushHandController::TriggerPressed()
{
	if (!StrokeClass) return;
	CurrentStroke = GetWorld()->SpawnActor<AStroke>(StrokeClass);
	CurrentStroke->SetActorLocation(GetCursorLocation());
}

void APaintBrushHandController::TriggerReleased()
{
	CurrentStroke = nullptr;
}

FVector APaintBrushHandController::GetCursorLocation()
{
	return GetActorLocation() + GetActorForwardVector() * StrokeDistance;
}

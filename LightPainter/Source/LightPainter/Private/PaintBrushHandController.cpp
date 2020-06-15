// Copyright Scott Kolby 2020

#include "PaintBrushHandController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Stroke.h"

#include "DrawDebugHelpers.h"

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
	
	UpdateStroke();
}

void APaintBrushHandController::UpdateStroke()
{
	if (DeleteModeActive)
	{
		DeleteStroke();
	}
	else if (CurrentStroke)
	{
		CurrentStroke->Update(GetCursorLocation());
	}
}

void APaintBrushHandController::TriggerPressed()
{
	if (DeleteModeEnabled)
	{
		DeleteModeActive = true;
	}
	else
	{
		SpawnStroke();
	}
}

void APaintBrushHandController::TriggerReleased()
{
	CurrentStroke = nullptr;
	DeleteModeActive = false;
}

void APaintBrushHandController::ToggleDelete()
{
	DeleteModeEnabled = !DeleteModeEnabled;
}

void APaintBrushHandController::SpawnStroke()
{
	if (!StrokeClass || DeleteModeActive) return;
	CurrentStroke = GetWorld()->SpawnActor<AStroke>(StrokeClass);
	CurrentStroke->SetActorLocation(GetCursorLocation());
}

void APaintBrushHandController::DeleteStroke()
{
	if (!DeleteModeActive) return;

	FHitResult HitResult;
	FVector Start = GetCursorLocation();
	FVector End = GetCursorLocation() + GetActorForwardVector() * StrokeDistance;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.f, 0, 2);
	if (bHit)
	{
		AStroke* Stroke = Cast<AStroke>(HitResult.GetActor());
		if (!Stroke) return;

		Stroke->Destroy();
	}
}

FVector APaintBrushHandController::GetCursorLocation()
{
	return GetActorLocation() + GetActorForwardVector() * StrokeDistance;
}

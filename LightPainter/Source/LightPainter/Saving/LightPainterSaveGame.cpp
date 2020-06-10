// Copyright Scott Kolby 2020


#include "LightPainterSaveGame.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Stroke.h"

ULightPainterSaveGame* ULightPainterSaveGame::Create()
{
	return Cast<ULightPainterSaveGame>(UGameplayStatics::CreateSaveGameObject(StaticClass()));
}

bool ULightPainterSaveGame::Save()
{
	return UGameplayStatics::SaveGameToSlot(this, TEXT("Scott"), 0);
}

ULightPainterSaveGame* ULightPainterSaveGame::Load()
{
	return Cast<ULightPainterSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("Scott"), 0));
}

void ULightPainterSaveGame::SerializeFromWorld(UWorld* World)
{
	Strokes.Empty();

	for (TActorIterator<AStroke> StrokeIterator(World); StrokeIterator; ++StrokeIterator)
	{
		Strokes.Add(StrokeIterator->SerializeToStruct());
	}
}

void ULightPainterSaveGame::DeserializeToWorld(UWorld* World)
{
	ClearWorld(World);
	
	for (FStrokeState StrokeState : Strokes)
	{
		AStroke::DeserializeFromStruct(World, StrokeState);
	}
}

void ULightPainterSaveGame::ClearWorld(UWorld* World)
{
	for (TActorIterator<AStroke> StrokeIterator(World); StrokeIterator; ++StrokeIterator)
	{
		StrokeIterator->Destroy();
	}
}

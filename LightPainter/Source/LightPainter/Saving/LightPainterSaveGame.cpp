// Copyright Scott Kolby 2020


#include "LightPainterSaveGame.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "LightPainterSaveGameIndex.h"
#include "Misc/Guid.h"
#include "Stroke.h"

ULightPainterSaveGame* ULightPainterSaveGame::Create()
{
	ULightPainterSaveGame* NewSaveGame = Cast<ULightPainterSaveGame>(UGameplayStatics::CreateSaveGameObject(StaticClass()));
	if (NewSaveGame)
	{
		NewSaveGame->CurrentSlotName = FGuid::NewGuid().ToString();
		if (!NewSaveGame->Save()) return nullptr;

		ULightPainterSaveGameIndex* SaveGameIndex = ULightPainterSaveGameIndex::Load();
		SaveGameIndex->AddSaveGame(NewSaveGame);
		SaveGameIndex->Save();
	}
	return NewSaveGame;
}

bool ULightPainterSaveGame::Save()
{
	return UGameplayStatics::SaveGameToSlot(this, CurrentSlotName, 0);
}

ULightPainterSaveGame* ULightPainterSaveGame::Load(FString CurrentSlotName)
{
	return Cast<ULightPainterSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
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

FString ULightPainterSaveGame::GetSlotName() const
{
	return CurrentSlotName;
}

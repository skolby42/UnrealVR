// Copyright Scott Kolby 2020


#include "LightPainterSaveGame.h"
#include "Kismet/GameplayStatics.h"

ULightPainterSaveGame* ULightPainterSaveGame::Create()
{
	return Cast<ULightPainterSaveGame>(UGameplayStatics::CreateSaveGameObject(StaticClass()));
}

bool ULightPainterSaveGame::Save()
{
	return UGameplayStatics::SaveGameToSlot(this, TEXT("Scott"), 0);
}
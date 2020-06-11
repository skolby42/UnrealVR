// Copyright Scott Kolby 2020


#include "LightPainterSaveGameIndex.h"
#include "LightPainterSaveGame.h"
#include "Kismet/GameplayStatics.h"


const FString ULightPainterSaveGameIndex::SLOT_NAME = "SaveGameIndex";

ULightPainterSaveGameIndex* ULightPainterSaveGameIndex::Load()
{
	auto SaveGameIndex = Cast<ULightPainterSaveGameIndex>(UGameplayStatics::LoadGameFromSlot(SLOT_NAME, 0));
	if (!SaveGameIndex)
	{
		SaveGameIndex = Cast<ULightPainterSaveGameIndex>(UGameplayStatics::CreateSaveGameObject(StaticClass()));
		SaveGameIndex->Save();
	}
	return SaveGameIndex;
}

bool ULightPainterSaveGameIndex::Save()
{
	return UGameplayStatics::SaveGameToSlot(this, SLOT_NAME, 0);
}

void ULightPainterSaveGameIndex::AddSaveGame(ULightPainterSaveGame* SaveGame)
{
	if (!SaveGame) return;

	SlotNames.Add(SaveGame->GetSlotName());
	Save();
}

TArray<FString> ULightPainterSaveGameIndex::GetSlotNames() const
{
	return SlotNames;
}

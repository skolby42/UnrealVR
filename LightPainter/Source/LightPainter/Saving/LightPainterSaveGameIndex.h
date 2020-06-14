// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LightPainterSaveGameIndex.generated.h"

class ULightPainterSaveGame;

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API ULightPainterSaveGameIndex : public USaveGame
{
	GENERATED_BODY()
	
public:
	static ULightPainterSaveGameIndex* Load();
	bool Save();
	void AddSaveGame(ULightPainterSaveGame* SaveGame);
	void RemoveSlot(FString SlotName);
	TArray<FString> GetSlotNames() const;

private:
	static const FString SLOT_NAME;

	UPROPERTY()
	TArray<FString> SlotNames;
};

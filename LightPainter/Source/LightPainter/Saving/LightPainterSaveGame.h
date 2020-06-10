// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LightPainterSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API ULightPainterSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	static ULightPainterSaveGame* Create();
	bool Save();
};

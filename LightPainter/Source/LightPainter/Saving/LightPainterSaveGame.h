// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LightPainterSaveGame.generated.h"

USTRUCT()
struct FStrokeState
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<class AStroke> Class;

	UPROPERTY()
	TArray<FVector> ControlPoints;
};

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
	static ULightPainterSaveGame* Load();

	void SerializeFromWorld(UWorld* World);
	void DeserializeToWorld(UWorld* World);

	void ClearWorld(UWorld* World);

private:
	// State
	UPROPERTY()
	TArray<FStrokeState> Strokes;
};

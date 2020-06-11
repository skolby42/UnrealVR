// Copyright Scott Kolby 2020


#include "PaintingGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/StereoLayerFunctionLibrary.h"
#include "LightPainter/Saving/LightPainterSaveGame.h"

void APaintingGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	SlotName = UGameplayStatics::ParseOption(Options, TEXT("SlotName"));
}

bool APaintingGameMode::Save()
{
	return false;
}

void APaintingGameMode::BeginPlay()
{
	Super::BeginPlay();

	Load();
	UStereoLayerFunctionLibrary::HideSplashScreen();
}

void APaintingGameMode::Load()
{
	ULightPainterSaveGame* SaveGame = ULightPainterSaveGame::Load(SlotName);
	if (SaveGame)
	{
		SaveGame->DeserializeToWorld(GetWorld());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save game %s not loaded"), *SlotName)
	}
}

// Copyright Scott Kolby 2020


#include "PaintingGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/StereoLayerFunctionLibrary.h"
#include "LightPainter/Saving/LightPainterSaveGame.h"

void APaintingGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	SlotName = UGameplayStatics::ParseOption(Options, TEXT("SlotName"));
}

void APaintingGameMode::Save()
{
	ULightPainterSaveGame* SaveGame = ULightPainterSaveGame::Load(SlotName);
	if (SaveGame)
	{
		SaveGame->SerializeFromWorld(GetWorld());
		SaveGame->Save();
	}
}

void APaintingGameMode::BeginPlay()
{
	Super::BeginPlay();

	Load();
	UStereoLayerFunctionLibrary::HideSplashScreen();
}

void APaintingGameMode::SaveAndExitLevel()
{
	Save();
	ReturnToMainMenu();
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

void APaintingGameMode::ReturnToMainMenu()
{
	UStereoLayerFunctionLibrary::ShowSplashScreen();  // Workaround for bug causing hang when opening level
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));
}

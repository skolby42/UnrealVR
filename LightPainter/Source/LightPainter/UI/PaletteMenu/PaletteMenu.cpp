// Copyright Scott Kolby 2020


#include "PaletteMenu.h"
#include "Components/Button.h"
#include "PaintingGameMode.h"

bool UPaletteMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (!ExitButton) return false;
	ExitButton->OnReleased.AddDynamic(this, &UPaletteMenu::ExitButtonReleased);

	return true;
}

void UPaletteMenu::ExitButtonReleased()
{
	auto GameMode = Cast<APaintingGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	GameMode->SaveAndExitLevel();
}
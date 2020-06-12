// Copyright Scott Kolby 2020


#include "PaletteMenu.h"
#include "Components/Button.h"
#include "PaintingGameMode.h"

bool UPaletteMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (!ExitButton) return false;
	ExitButton->OnPressed.AddDynamic(this, &UPaletteMenu::ExitButtonPressed);

	return true;
}

void UPaletteMenu::ExitButtonPressed()
{
	auto GameMode = Cast<APaintingGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	GameMode->SaveAndExitLevel();
}
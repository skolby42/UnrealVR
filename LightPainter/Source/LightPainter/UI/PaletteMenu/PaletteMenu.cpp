// Copyright Scott Kolby 2020


#include "PaletteMenu.h"
#include "Components/Button.h"
#include "PaintingGameMode.h"
#include "HandControllerBase.h"

bool UPaletteMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (!ExitButton) return false;
	ExitButton->OnReleased.AddDynamic(this, &UPaletteMenu::ExitButtonReleased);

	if (!DeleteButton) return false;
	DeleteButton->OnReleased.AddDynamic(this, &UPaletteMenu::DeleteButtonReleased);

	return true;
}

void UPaletteMenu::SetParentController(AHandControllerBase* Controller)
{
	ParentController = Controller;
}

void UPaletteMenu::ExitButtonReleased()
{
	auto GameMode = Cast<APaintingGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	GameMode->SaveAndExitLevel();
}

void UPaletteMenu::DeleteButtonReleased()
{
	if (!ParentController) return;
	UE_LOG(LogTemp, Warning, TEXT("DeleteButtonReleased"))
	ParentController->ToggleDelete();
}
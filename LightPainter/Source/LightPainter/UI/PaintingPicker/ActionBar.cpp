// Copyright Scott Kolby 2020


#include "ActionBar.h"
#include "PaintingPicker.h"
#include "Kismet/KismetSystemLibrary.h"

bool UActionBar::Initialize()
{
	if (!Super::Initialize()) return false;

	if (!AddButton) return false;
	AddButton->OnClicked.AddDynamic(this, &UActionBar::AddButtonClicked);

	if (!DeleteButton) return false;
	DeleteButton->OnClicked.AddDynamic(this, &UActionBar::DeleteButtonClicked);

	if (!ExitButton) return false;
	ExitButton->OnClicked.AddDynamic(this, &UActionBar::ExitButtonClicked);

	return true;
}

void UActionBar::SetParentPicker(APaintingPicker* NewParentPicker)
{
	ParentPicker = NewParentPicker;
}

void UActionBar::AddButtonClicked()
{
	if (!ParentPicker) return;
	ParentPicker->AddPainting();
}

void UActionBar::DeleteButtonClicked()
{
	if (!ParentPicker) return;
	ParentPicker->ToggleDeleteMode();
}

void UActionBar::ExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}

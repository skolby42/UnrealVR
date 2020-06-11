// Copyright Scott Kolby 2020


#include "ActionBar.h"

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

void UActionBar::AddButtonClicked()
{
	
}

void UActionBar::DeleteButtonClicked()
{
	
}

void UActionBar::ExitButtonClicked()
{
	
}

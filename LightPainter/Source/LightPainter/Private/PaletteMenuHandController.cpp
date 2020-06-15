// Copyright Scott Kolby 2020


#include "PaletteMenuHandController.h"
#include "Components/WidgetComponent.h"
#include "LightPainter/UI/PaletteMenu/PaletteMenu.h"

APaletteMenuHandController::APaletteMenuHandController()
{
	PrimaryActorTick.bCanEverTick = true;

	PaletteMenu = CreateDefaultSubobject<UWidgetComponent>(TEXT("PaletteMenu"));
	PaletteMenu->SetupAttachment(GetRootComponent());
}

void APaletteMenuHandController::BeginPlay()
{
	Super::BeginPlay();

	UPaletteMenu* PaletteMenuWidget = Cast<UPaletteMenu>(PaletteMenu->GetUserWidgetObject());
	if (PaletteMenuWidget)
	{
		PaletteMenuWidget->SetParentController(this);
	}
}

void APaletteMenuHandController::ToggleDelete()
{
	if (!GetPairedController()) return;
	GetPairedController()->ToggleDelete();
}

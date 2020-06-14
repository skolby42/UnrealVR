// Copyright Scott Kolby 2020


#include "PaintingPicker.h"
#include "Components/WidgetComponent.h"
#include "LightPainter/Saving/LightPainterSaveGame.h"
#include "LightPainter/Saving/LightPainterSaveGameIndex.h"
#include "PaintingGrid.h"
#include "ActionBar.h"

// Sets default values
APaintingPicker::APaintingPicker()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PaintingGrid = CreateDefaultSubobject<UWidgetComponent>(TEXT("PaintingGrid"));
	PaintingGrid->SetupAttachment(GetRootComponent());

	ActionBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("ActionBar"));
	ActionBar->SetupAttachment(GetRootComponent());
}

void APaintingPicker::BeginPlay()
{
	Super::BeginPlay();
	
	LoadActionBar();
	Refresh();
}

void APaintingPicker::AddPainting()
{
	ULightPainterSaveGame::Create();
	Refresh();
}

void APaintingPicker::ToggleDeleteMode()
{
	if (!GetPaintingGrid()) return;
	GetPaintingGrid()->ToggleDeleteMode();
}

void APaintingPicker::UpdateCurrentPage(int32 Offset)
{
	if (!GetPaintingGrid()) return;
	GetPaintingGrid()->UpdateCurrentPage(Offset);
}

void APaintingPicker::LoadActionBar()
{
	if (!ActionBar) return;

	UActionBar* ActionBarWidget = Cast<UActionBar>(ActionBar->GetUserWidgetObject());
	if (!ActionBarWidget) return;
	
	ActionBarWidget->SetParentPicker(this);
}

void APaintingPicker::Refresh()
{
	if (!GetPaintingGrid()) return;

	GetPaintingGrid()->Refresh();
}

UPaintingGrid* APaintingPicker::GetPaintingGrid() const
{
	if (!PaintingGrid) return nullptr;
	return Cast<UPaintingGrid>(PaintingGrid->GetUserWidgetObject());
}


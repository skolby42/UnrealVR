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
	LoadPaintingGrid();
}

void APaintingPicker::AddPainting()
{
	ULightPainterSaveGame::Create();
	LoadPaintingGrid();
	LoadPaginationDots();
}

void APaintingPicker::ToggleDeleteMode()
{
	
}

void APaintingPicker::LoadActionBar()
{
	if (!ActionBar) return;

	UActionBar* ActionBarWidget = Cast<UActionBar>(ActionBar->GetUserWidgetObject());
	if (!ActionBarWidget) return;
	
	ActionBarWidget->SetParentPicker(this);
}

void APaintingPicker::LoadPaintingGrid()
{
	if (!PaintingGrid) return;

	ULightPainterSaveGameIndex* SaveGameIndex = ULightPainterSaveGameIndex::Load();
	if (!SaveGameIndex) return;

	UPaintingGrid* PaintingGridWidget = Cast<UPaintingGrid>(PaintingGrid->GetUserWidgetObject());
	if (!PaintingGridWidget) return;

	PaintingGridWidget->ClearPaintings();

	TArray<FString> SlotNames = SaveGameIndex->GetSlotNames();
	for (int32 i = 0; i < SlotNames.Num(); ++i)
	{
		PaintingGridWidget->AddPainting(i, SlotNames[i]);
	}
}

void APaintingPicker::LoadPaginationDots()
{
	UPaintingGrid* PaintingGridWidget = Cast<UPaintingGrid>(PaintingGrid->GetUserWidgetObject());
	if (!PaintingGridWidget) return;

	PaintingGridWidget->ClearPaginationDots();
	PaintingGridWidget->AddPaginationDot(true);
	PaintingGridWidget->AddPaginationDot(false);
	PaintingGridWidget->AddPaginationDot(false);
}


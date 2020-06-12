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
	
}

void APaintingPicker::UpdateCurrentPage(int32 Offset)
{
	CurrentPage = FMath::Clamp(CurrentPage + Offset, 0, GetNumberOfPages() - 1);
	Refresh();
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
	RefreshPaginationDots();
	RefreshPaintingGrid();
}

void APaintingPicker::RefreshPaintingGrid()
{
	if (!PaintingGrid) return;

	ULightPainterSaveGameIndex* SaveGameIndex = ULightPainterSaveGameIndex::Load();
	if (!SaveGameIndex) return;

	if (!GetPaintingGrid()) return;

	GetPaintingGrid()->ClearPaintings();

	int32 StartOffset = CurrentPage * GetPaintingGrid()->GetNumberOfSlots();
	TArray<FString> SlotNames = SaveGameIndex->GetSlotNames();
	for (int32 i = 0; i < GetPaintingGrid()->GetNumberOfSlots(); ++i)
	{
		int32 CurrentSlot = i + StartOffset;
		if (CurrentSlot == SlotNames.Num()) break;
		GetPaintingGrid()->AddPainting(i, SlotNames[CurrentSlot]);
	}
}

void APaintingPicker::RefreshPaginationDots()
{
	if (!GetPaintingGrid()) return;

	GetPaintingGrid()->ClearPaginationDots();

	for (int32 i = 0; i < GetNumberOfPages(); ++i)
	{
		bool bActive = i == CurrentPage;
		GetPaintingGrid()->AddPaginationDot(bActive);
	}
}

int32 APaintingPicker::GetNumberOfPages() const
{
	if (!PaintingGrid) return 0;

	if (!GetPaintingGrid()) return 0;

	ULightPainterSaveGameIndex* SaveGameIndex = ULightPainterSaveGameIndex::Load();
	if (!SaveGameIndex) return 0;

	int32 TotalSlots = SaveGameIndex->GetSlotNames().Num();
	int32 SlotsPerPage = GetPaintingGrid()->GetNumberOfSlots();

	if (TotalSlots == 0)
		TotalSlots = 1;

	if (SlotsPerPage == 0) 
		SlotsPerPage = 1;

	return FMath::RoundFromZero(float(TotalSlots) / (float)SlotsPerPage);
}

UPaintingGrid* APaintingPicker::GetPaintingGrid() const
{
	return Cast<UPaintingGrid>(PaintingGrid->GetUserWidgetObject());
}


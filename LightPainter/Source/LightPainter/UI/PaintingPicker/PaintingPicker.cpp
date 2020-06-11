// Copyright Scott Kolby 2020


#include "PaintingPicker.h"
#include "Components/WidgetComponent.h"
#include "LightPainter/Saving/LightPainterSaveGameIndex.h"
#include "PaintingGrid.h"

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
	
	InitializePaintingGrid();
}

void APaintingPicker::InitializePaintingGrid()
{
	if (!PaintingGrid) return;

	ULightPainterSaveGameIndex* SaveGameIndex = ULightPainterSaveGameIndex::Load();
	if (!SaveGameIndex) return;

	UPaintingGrid* PaintingGridWidget = Cast<UPaintingGrid>(PaintingGrid->GetUserWidgetObject());
	if (!PaintingGridWidget) return;

	TArray<FString> SlotNames = SaveGameIndex->GetSlotNames();
	for (int32 i = 0; i < SlotNames.Num(); ++i)
	{
		PaintingGridWidget->AddPainting(i, SlotNames[i]);
	}
}


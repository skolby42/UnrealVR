// Copyright Scott Kolby 2020


#include "PaintingPicker.h"
#include "PaintingGrid.h"
#include "Components/WidgetComponent.h"

// Sets default values
APaintingPicker::APaintingPicker()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PaintingGrid = CreateDefaultSubobject<UWidgetComponent>(TEXT("PaintingGrid"));
	PaintingGrid->SetupAttachment(GetRootComponent());

	ActionBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("ActionBar"));
	ActionBar->SetupAttachment(GetRootComponent());
}

void APaintingPicker::BeginPlay()
{
	Super::BeginPlay();
	
	if (PaintingGrid)
	{
		UPaintingGrid* PaintingGridComponent = Cast<UPaintingGrid>(PaintingGrid->GetUserWidgetObject());
		PaintingGridComponent->AddPainting();
	}
}


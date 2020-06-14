// Copyright Scott Kolby 2020


#include "PaintingGrid.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "LightPainter/Saving/LightPainterSaveGameIndex.h"
#include "PaintingGridCard.h"
#include "PaginationDot.h"


bool UPaintingGrid::Initialize()
{
	if (!Super::Initialize()) return false;

	UpdateBackgroundColor();
	return true;
}

void UPaintingGrid::Refresh()
{
	RefreshPaginationDots();
	RefreshPaintingGrid();
}

void UPaintingGrid::RefreshPaintingGrid()
{
	ULightPainterSaveGameIndex* SaveGameIndex = ULightPainterSaveGameIndex::Load();
	if (!SaveGameIndex) return;

	ClearPaintings();

	int32 StartOffset = CurrentPage * GetNumberOfSlots();
	TArray<FString> SlotNames = SaveGameIndex->GetSlotNames();
	for (int32 i = 0; i < GetNumberOfSlots(); ++i)
	{
		int32 CurrentSlot = i + StartOffset;
		if (CurrentSlot == SlotNames.Num()) break;
		AddPainting(i, SlotNames[CurrentSlot], FString::FromInt(CurrentSlot + 1));
	}
}

void UPaintingGrid::RefreshPaginationDots()
{
	ClearPaginationDots();

	int32 NumberOfPages = GetNumberOfPages();
	CurrentPage = FMath::Clamp(CurrentPage, 0, NumberOfPages - 1);

	for (int32 i = 0; i < NumberOfPages; ++i)
	{
		bool bActive = i == CurrentPage;
		AddPaginationDot(bActive);
	}
}

int32 UPaintingGrid::GetNumberOfPages() const
{
	if (!PaintingGrid) return 0;

	ULightPainterSaveGameIndex* SaveGameIndex = ULightPainterSaveGameIndex::Load();
	if (!SaveGameIndex) return 0;

	int32 TotalSlots = SaveGameIndex->GetSlotNames().Num();
	int32 SlotsPerPage = GetNumberOfSlots();

	if (TotalSlots == 0)
		TotalSlots = 1;

	if (SlotsPerPage == 0)
		SlotsPerPage = 1;

	return FMath::RoundFromZero(float(TotalSlots) / (float)SlotsPerPage);
}

void UPaintingGrid::AddPainting(int32 PaintingIndex, FString PaintingName, FString DisplayName)
{
	if (!PaintingGrid) return;

	UPaintingGridCard* PaintingGridCardWidget = CreateWidget<UPaintingGridCard>(GetWorld(), GridCardClass);
	if (!PaintingGridCardWidget) return;
	PaintingGridCardWidget->SetPaintingName(PaintingName);
	PaintingGridCardWidget->SetDisplayName(DisplayName);
	PaintingGridCardWidget->SetParentGrid(this);
	
	USizeBox* CardContainer = Cast<USizeBox>(PaintingGrid->GetChildAt(PaintingIndex));
	if (!CardContainer) return;

	CardContainer->AddChild(PaintingGridCardWidget);
}

void UPaintingGrid::ClearPaintings()
{
	if (!PaintingGrid) return;

	for (int i = 0; i < PaintingGrid->GetChildrenCount(); ++i)
	{
		USizeBox* CardContainer = Cast<USizeBox>(PaintingGrid->GetChildAt(i));
		if (!CardContainer) continue;

		CardContainer->ClearChildren();
	}
}

void UPaintingGrid::AddPaginationDot(bool Active)
{
	if (!PaginationDots) return;

	UPaginationDot* PaginationDotWidget = CreateWidget<UPaginationDot>(GetWorld(), PaginationDotClass);
	if (!PaginationDotWidget) return;
	PaginationDotWidget->SetActive(Active);

	UHorizontalBoxSlot* HorizontalBoxSlot = PaginationDots->AddChildToHorizontalBox(PaginationDotWidget);
	if (!HorizontalBoxSlot) return;
	HorizontalBoxSlot->SetPadding(FMargin(PaginationDotPadding, 0.f));
}

void UPaintingGrid::ClearPaginationDots()
{
	if (!PaginationDots) return;
	PaginationDots->ClearChildren();
}

int32 UPaintingGrid::GetNumberOfSlots() const
{
	if (!PaintingGrid) return 0;
	return PaintingGrid->GetChildrenCount();
}

void UPaintingGrid::ToggleDeleteMode()
{
	DeleteModeActive = !DeleteModeActive;
	UpdateBackgroundColor();
}

bool UPaintingGrid::GetDeleteModeActive()
{
	return DeleteModeActive;
}

void UPaintingGrid::UpdateCurrentPage(int32 Offset)
{
	CurrentPage = FMath::Clamp(CurrentPage + Offset, 0, GetNumberOfPages() - 1);
	Refresh();
}

void UPaintingGrid::UpdateBackgroundColor()
{
	if (!BackgroundImage) return;

	FLinearColor Color = DeleteModeActive ? DeleteModeBackgroundColor : DefaultBackgroundColor;
	BackgroundImage->SetColorAndOpacity(Color);
}

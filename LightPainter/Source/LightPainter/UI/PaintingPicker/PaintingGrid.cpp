// Copyright Scott Kolby 2020


#include "PaintingGrid.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "PaintingGridCard.h"
#include "PaginationDot.h"


void UPaintingGrid::AddPainting(int32 PaintingIndex, FString PaintingName, FString DisplayName)
{
	if (!PaintingGrid) return;

	UPaintingGridCard* PaintingGridCardWidget = CreateWidget<UPaintingGridCard>(GetWorld(), GridCardClass);
	if (!PaintingGridCardWidget) return;
	PaintingGridCardWidget->SetPaintingName(PaintingName);
	PaintingGridCardWidget->SetDisplayName(DisplayName);
	
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

void UPaintingGrid::AddPaginationDot(bool bActive)
{
	if (!PaginationDots) return;

	UPaginationDot* PaginationDotWidget = CreateWidget<UPaginationDot>(GetWorld(), PaginationDotClass);
	if (!PaginationDotWidget) return;
	PaginationDotWidget->SetActive(bActive);

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

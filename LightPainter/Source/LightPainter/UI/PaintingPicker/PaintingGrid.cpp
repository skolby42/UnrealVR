// Copyright Scott Kolby 2020


#include "PaintingGrid.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"
#include "PaintingGridCard.h"


void UPaintingGrid::AddPainting(int32 PaintingIndex, FString PaintingName)
{
	if (!PaintingGrid) return;

	UPaintingGridCard* PaintingGridCardWidget = CreateWidget<UPaintingGridCard>(GetWorld(), GridCardClass);
	if (!PaintingGridCardWidget) return;
	PaintingGridCardWidget->SetPaintingName(PaintingName);
	
	USizeBox* CardContainer = Cast<USizeBox>(PaintingGrid->GetChildAt(PaintingIndex));
	if (!CardContainer) return;

	CardContainer->ClearChildren();
	CardContainer->AddChild(PaintingGridCardWidget);
}
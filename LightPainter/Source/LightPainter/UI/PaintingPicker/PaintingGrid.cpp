// Copyright Scott Kolby 2020


#include "PaintingGrid.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"


void UPaintingGrid::AddPainting()
{
	if (!PaintingGrid) return;

	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), GridCardClass);
	if (!NewWidget) return;

	int32 ChildCount = PaintingGrid->GetChildrenCount();
	if (ChildCount == 0) return;

	USizeBox* CardContainer = Cast<USizeBox>(PaintingGrid->GetChildAt(0));
	if (!CardContainer) return;

	CardContainer->AddChild(NewWidget);
}
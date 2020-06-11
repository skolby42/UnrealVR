// Copyright Scott Kolby 2020


#include "PaintingGrid.h"
#include "Components/UniformGridPanel.h"

void UPaintingGrid::AddPainting()
{
	if (PaintingGrid)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddPainting"));
	}
}
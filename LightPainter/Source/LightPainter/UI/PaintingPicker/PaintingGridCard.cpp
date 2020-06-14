// Copyright Scott Kolby 2020


#include "PaintingGridCard.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/StereoLayerFunctionLibrary.h"
#include "LightPainter/Saving/LightPainterSaveGameIndex.h"
#include "PaintingGrid.h"


void UPaintingGridCard::SetPaintingName(FString NewPaintingName)
{
	PaintingName = NewPaintingName;
	CardButton->OnClicked.AddDynamic(this, &UPaintingGridCard::CardButtonClicked);
}

void UPaintingGridCard::SetDisplayName(FString NewDisplayName)
{
	DisplayName->SetText(FText::FromString(NewDisplayName));
}

void UPaintingGridCard::SetParentGrid(UPaintingGrid* NewParentGrid)
{
	ParentGrid = NewParentGrid;
}

void UPaintingGridCard::CardButtonClicked()
{
	if (ParentGrid && ParentGrid->GetDeleteModeActive())
	{
		DeletePainting();
		ParentGrid->Refresh();
	}
	else
	{
		OpenPainting();
	}
}

void UPaintingGridCard::OpenPainting()
{
	UStereoLayerFunctionLibrary::ShowSplashScreen();  // Workaround for bug causing hang when opening level
	// TODO refactor magic string
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Canvas"), true, TEXT("SlotName=" + PaintingName));
}

void UPaintingGridCard::DeletePainting()
{
	if (!UGameplayStatics::DeleteGameInSlot(PaintingName, 0)) return;

	ULightPainterSaveGameIndex* SaveGameIndex = ULightPainterSaveGameIndex::Load();
	SaveGameIndex->RemoveSlot(PaintingName);
}

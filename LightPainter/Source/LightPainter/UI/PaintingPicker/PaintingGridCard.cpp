// Copyright Scott Kolby 2020


#include "PaintingGridCard.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/StereoLayerFunctionLibrary.h"

void UPaintingGridCard::SetPaintingName(FString NewPaintingName)
{
	PaintingName = NewPaintingName;
	CardButton->OnClicked.AddDynamic(this, &UPaintingGridCard::CardButtonClicked);
}

void UPaintingGridCard::SetDisplayName(FString NewDisplayName)
{
	DisplayName->SetText(FText::FromString(NewDisplayName));
}

void UPaintingGridCard::CardButtonClicked()
{
	UStereoLayerFunctionLibrary::ShowSplashScreen();  // Workaround for bug causing hang when opening level
	// TODO refactor magic string
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Canvas"), true, TEXT("SlotName=" + PaintingName));
}

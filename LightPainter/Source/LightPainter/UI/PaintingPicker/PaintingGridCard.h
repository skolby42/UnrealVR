// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "PaintingGridCard.generated.h"

class UPaintingGrid;

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API UPaintingGridCard : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPaintingName(FString NewPaintingName);
	void SetDisplayName(FString NewDisplayName);
	void SetParentGrid(UPaintingGrid* NewParentGrid);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	UTextBlock* DisplayName;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	UButton* CardButton;

private:
	UFUNCTION()
	void CardButtonClicked();

	void OpenPainting();
	void DeletePainting();

	UPROPERTY()
	UPaintingGrid* ParentGrid;

	// State
	FString PaintingName;
	
};

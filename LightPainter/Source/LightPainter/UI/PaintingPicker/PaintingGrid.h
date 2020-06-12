// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaintingGrid.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API UPaintingGrid : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddPainting(int32 PaintingIndex, FString PaintingName);
	void ClearPaintings();
	void AddPaginationDot(bool bIsActive);
	void ClearPaginationDots();

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	class UUniformGridPanel* PaintingGrid;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	class UHorizontalBox* PaginationDots;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPaintingGridCard> GridCardClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPaginationDot> PaginationDotClass;
};

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
	bool Initialize() override;
	void Refresh();
	void ToggleDeleteMode();
	bool GetDeleteModeActive();
	void UpdateCurrentPage(int32 Offset);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	class UUniformGridPanel* PaintingGrid;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	class UImage* BackgroundImage;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	class UHorizontalBox* PaginationDots;

private:
	void RefreshPaintingGrid();
	void RefreshPaginationDots();
	int32 GetNumberOfPages() const;
	void UpdateBackgroundColor();
	void AddPainting(int32 PaintingIndex, FString PaintingName, FString DisplayName);
	void ClearPaintings();
	void AddPaginationDot(bool Active);
	void ClearPaginationDots();
	int32 GetNumberOfSlots() const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPaintingGridCard> GridCardClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPaginationDot> PaginationDotClass;

	UPROPERTY(EditDefaultsOnly)
	float PaginationDotPadding = 8.f;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor DefaultBackgroundColor = FLinearColor(0.07, 0.07, 0.07, 0.75);

	UPROPERTY(EditDefaultsOnly)
	FLinearColor DeleteModeBackgroundColor = FLinearColor(0.7, 0, 0, 0.75);

	// State
	bool DeleteModeActive = false;
	int32 CurrentPage = 0;
};

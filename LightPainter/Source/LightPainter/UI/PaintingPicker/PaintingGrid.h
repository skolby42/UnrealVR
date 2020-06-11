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

	UFUNCTION(BlueprintCallable)
	void AddPainting();

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	class UUniformGridPanel* PaintingGrid;
};

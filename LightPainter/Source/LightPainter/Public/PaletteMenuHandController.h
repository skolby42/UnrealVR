// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "HandControllerBase.h"
#include "PaletteMenuHandController.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API APaletteMenuHandController : public AHandControllerBase
{
	GENERATED_BODY()

public:
	APaletteMenuHandController();

protected:
	void BeginPlay() override;

private:
	void ToggleDelete() override;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* PaletteMenu = nullptr;
};

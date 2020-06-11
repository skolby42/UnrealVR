// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintingPicker.generated.h"

UCLASS()
class LIGHTPAINTER_API APaintingPicker : public AActor
{
	GENERATED_BODY()
	
public:	
	APaintingPicker();
	void AddPainting();
	void ToggleDeleteMode();

protected:
	virtual void BeginPlay() override;
	
private:
	void LoadActionBar();
	void LoadPaintingGrid();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* PaintingGrid = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* ActionBar = nullptr;
};

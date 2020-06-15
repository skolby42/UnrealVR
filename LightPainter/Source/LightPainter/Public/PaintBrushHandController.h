// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HandControllerBase.h"
#include "PaintBrushHandController.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API APaintBrushHandController : public AHandControllerBase
{
	GENERATED_BODY()

public:
	APaintBrushHandController();

	virtual void Tick(float DeltaTime) override;
	void UpdateStroke();
	void TriggerPressed() override;
	void SpawnStroke();
	void DeleteStroke();
	void TriggerReleased() override;
	void ToggleDelete() override;

protected:
	virtual void BeginPlay() override;

private:
	FVector GetCursorLocation();

	// Default sub object
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AStroke> StrokeClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float StrokeDistance = 5.f;

	//State
	UPROPERTY()
	AStroke* CurrentStroke = nullptr;

	bool DeleteModeActive = false;
	bool DeleteModeEnabled = false;
};

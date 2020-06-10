// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintBrushHandController.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API APaintBrushHandController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APaintBrushHandController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SetHand(EControllerHand Hand);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateControllerMesh(EControllerHand Hand);

	UFUNCTION(BlueprintPure)
	class UStaticMeshComponent* GetControllerMesh();

	void TriggerPressed();
	void TriggerReleased();

private:

	FVector GetCursorLocation();

private:

	// Default sub object
	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* MotionController = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ControllerMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AStroke> StrokeClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float StrokeDistance = 5.f;

	//State
	UPROPERTY()
	AStroke* CurrentStroke = nullptr;
};

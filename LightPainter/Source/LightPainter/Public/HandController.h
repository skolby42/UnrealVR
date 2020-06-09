// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HandController.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API AHandController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHandController();

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

private:
	// Default sub object
	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* MotionController = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ControllerMesh = nullptr;
};

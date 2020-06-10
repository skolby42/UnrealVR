// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HandControllerBase.generated.h"

UCLASS()
class LIGHTPAINTER_API AHandControllerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandControllerBase();

	virtual void Tick(float DeltaTime) override;

	void SetHand(EControllerHand Hand);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateControllerMesh(EControllerHand Hand);

	UFUNCTION(BlueprintPure)
	class UStaticMeshComponent* GetControllerMesh();

	virtual void TriggerPressed();
	virtual void TriggerReleased();

protected:
	virtual void BeginPlay() override;

private:
	// Default sub object
	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* MotionController = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ControllerMesh = nullptr;
};

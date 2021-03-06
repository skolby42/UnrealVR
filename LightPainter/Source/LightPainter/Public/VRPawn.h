// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

UCLASS()
class LIGHTPAINTER_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	AVRPawn();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:

	void CreateHandControllers();
	void PaginateRight(float AxisValue);

	// Pass through methods
	void TriggerPressed();
	void TriggerReleased();

private:

	void UpdateCurrentPage(int32 Offset);

	// Components
	UPROPERTY(VisibleAnywhere)
	USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera = nullptr;

	// References
	UPROPERTY()
	class AHandControllerBase* LeftController = nullptr;
	
	UPROPERTY()
	class AHandControllerBase* RightController = nullptr;

	// Config
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHandControllerBase> LeftHandControllerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHandControllerBase> RightHandControllerClass;

	UPROPERTY(EditDefaultsOnly)
	float PaginationThumbstickThreshold = 0.8f;

	// State
	int32 LastPaginationOffset = 0;
};

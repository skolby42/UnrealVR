// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

class AHandController;
class UAnimInstance;
class UCameraComponent;
class UPostProcessComponent;

UCLASS()
class INTERACTIONSANDBOX_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVRCharacter();

protected:
	virtual void BeginPlay() override;
	void CreateHandControllers();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void TurnRight(float AxisValue);
	void SnapTurnRight();
	void SnapTurnLeft();
	void SnapTurn(float Rotation);
	void SyncActorToPlayspaceMovement();
	void StartFade(float FromAlpha, float ToAlpha, float FadeDuration);

	void CreateBlinkerMaterialInstance();
	void UpdateBlinkers();
	FVector2D GetBlinkerCenter();

	// Pass through methods
	void GripLeft();
	void ReleaseLeft();
	void GripRight();
	void ReleaseRight();
	void PressLeftTrigger();
	void ReleaseLeftTrigger();
	void PressRightTrigger();
	void ReleaseRightTrigger();
	void TeleportRight(float AxisValue);
	void TeleportUp(float AxisValue);
	void ReloadLeft();
	void ReloadRight();

private:

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera = nullptr;

	UPROPERTY()
	AHandController* LeftController = nullptr;

	UPROPERTY()
	AHandController* RightController = nullptr;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* VRRoot = nullptr;

	UPROPERTY(VisibleAnywhere)
	UPostProcessComponent* PostProcessComponent = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* BlinkerMaterialInstance = nullptr;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Turning")
	float TurnDegreesPerSecond = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Turning")
	bool bDoSnapTurn = true;

	UPROPERTY(EditDefaultsOnly, Category = "Turning")
	float SnapTurnFadeDuration = 0.25f;

	// Configuration properties
	UPROPERTY(EditDefaultsOnly, Category = "Blinkers")
	bool bBlinkersUseMovementDirection = false;

	UPROPERTY(EditDefaultsOnly, Category = "Blinkers")
	UMaterialInterface* BlinkerMaterialBase = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Blinkers")
	UCurveFloat* BlinkerRadiusVsVelocity = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHandController> HandControllerClass = nullptr;

	// State properties
	float BlinkerSpeedOverride = 0.f;
};

// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

class UCameraComponent;
class UPostProcessComponent;

UCLASS()
class ARCHITECTUREEXPLORER_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void SyncActorToPlayspaceMovement();
	void StartFade(float FromAlpha, float ToAlpha);
	void BeginTeleport();
	void EndTeleport();
	bool FindTeleportDestination(FVector& OutLocation) const;
	bool FindTeleportDestinationHMD(FVector& OutLocation) const;
	void UpdateDestinationMarker();
	void CreateBlinkerMaterialInstance();
	void UpdateBlinkers();

	// Get blinker center from movement direction
	FVector2D GetBlinkerCenter();


	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera = nullptr;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* VRRoot = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DestinationMarker = nullptr;

	UPROPERTY(VisibleAnywhere)
	UPostProcessComponent* PostProcessComponent = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* BlinkerMaterialBase = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* BlinkerMaterialInstance = nullptr;

	UPROPERTY(EditAnywhere)
	UCurveFloat* BlinkerRadiusVsVelocity = nullptr;

	UPROPERTY()
	class UMotionControllerComponent* LeftMotionController = nullptr;

	UPROPERTY()
	class UMotionControllerComponent* RightMotionController = nullptr;



	UPROPERTY(EditDefaultsOnly)
	float MaxTeleportDistance = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	float TeleportFadeDuration = 1.f;

	UPROPERTY(EditDefaultsOnly)
	FVector TeleportProjectionExtent = FVector(100.f, 100.f, 50.f);

	UPROPERTY(EditDefaultsOnly)
	float TeleportParabolaVelocity = 1000.f;
};

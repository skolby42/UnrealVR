// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

class AHandController;
class UCameraComponent;
class UPostProcessComponent;
class USplineComponent;
class USplineMeshComponent;

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

	void CreateHandControllers();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
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
	void ActivateTeleport();
	void BeginTeleport();
	void EndTeleport();
	bool FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation) const;
	bool FindTeleportDestinationHMD(FVector& OutLocation) const;
	void DrawTeleportArc(const TArray<FVector>& Path);
	void UpdateTeleportPath(const TArray<FVector>& Path);
	void UpdateDestinationMarker();
	void CreateBlinkerMaterialInstance();
	void UpdateBlinkers();
	FVector2D GetBlinkerCenter();

	// Pass through methods
	void GripLeft();
	void ReleaseLeft();
	void GripRight();
	void ReleaseRight();

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
	USplineComponent* TeleportPath = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DestinationMarker = nullptr;

	UPROPERTY(VisibleAnywhere)
	UPostProcessComponent* PostProcessComponent = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* BlinkerMaterialInstance = nullptr;

	UPROPERTY()
	TArray<USplineMeshComponent*> TeleportArcMeshPool;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Turning")
	float TurnDegreesPerSecond = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Turning")
	bool bDoSnapTurn = true;

	UPROPERTY(EditDefaultsOnly, Category = "Turning")
	float SnapTurnFadeDuration = 0.25f;

	// Configuration properties
	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	float TeleportFadeDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	FVector TeleportProjectionExtent = FVector(100.f, 100.f, 50.f);

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	float TeleportProjectileRadius = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	float TeleportProjectileSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	float TeleportSimulationTime = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	UStaticMesh* TeleportArcMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	UMaterialInterface* TeleportArcMaterial = nullptr;

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
	bool bIsTeleportActive = false;
};

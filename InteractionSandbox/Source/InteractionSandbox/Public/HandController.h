// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HandController.generated.h"

class UHandAnimInstance;
class UHapticFeedbackEffect_Curve;
class UMotionControllerComponent;
class UPhysicsHandleComponent;
class USphereComponent;
class USplineComponent;
class USplineMeshComponent;

UCLASS()
class INTERACTIONSANDBOX_API AHandController : public AActor
{
	GENERATED_BODY()
	
public:	
	AHandController();
	
	virtual void Tick(float DeltaTime) override;

	void SetHand(EControllerHand Hand);
	void PairController(AHandController* OtherController);
	//bool FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation) const;
	
	bool IsHoldingComponent(UPrimitiveComponent* OtherComponent) const;
	bool IsHoldingActor(AActor* OtherActor) const;
	bool IsHoldingObject() const;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateControllerMesh(EControllerHand Hand);

	UFUNCTION(BlueprintPure)
	UStaticMeshComponent* GetControllerMesh();

	void GripPressed();
	void GripReleased();
	void TriggerPressed();
	void TriggerReleased();
	void SetTeleportRight(float AxisValue);
	void SetTeleportUp(float AxisValue);

	float GetThumbDeadZone();
	bool IsTeleportActive();

	void UpdateSkeletalMesh(EControllerHand Hand);
	void SetControllerMeshVisibility(bool IsVisible);
	void SetSkeletalMeshVisibility(bool IsVisible);

protected:
	virtual void BeginPlay() override;

private:
	// Callbacks
	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Helpers
	void ControllerRumble() const;
	class AActor* GetOverlappingPickupActor() const;
	AActor* GetOverlappingActorWithTag(const FName& Tag) const;
	UPrimitiveComponent* GetOverlappingComponentWithTag(const FName& Tag) const;
	UHandAnimInstance* GetHandAnimInstance() const;
	void StartFade(float FromAlpha, float ToAlpha, float FadeDuration);

	bool CanClimb() const;
	bool CanCarry() const;
	bool CanGrab() const;
	
	void StartCarry();
	void UpdateCarry();
	void FinishCarry();

	void StartGrab();
	void UpdateGrab();
	void FinishGrab();
	
	void StartClimb();
	void UpdateClimb();
	void FinishClimb();

	bool AttachActor();
	bool ReleaseActor();

	void PickUpComponent(UPrimitiveComponent* Component);
	void GrabComponent(UPrimitiveComponent* Component);

	void UpdateGripHeldAnim(bool GripHeld);
	void UpdateCanGrabAnim(bool CanGrab);
	void UpdateGripTypeAnim();

	void ActivateTeleport();
	void BeginTeleport();
	void EndTeleport();
	void SetTeleportEnabled(bool bEnabled);
	void SetTeleportLocked(bool bLocked);
	bool FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation) const;
	void DrawTeleportArc(const TArray<FVector>& Path);
	void UpdateTeleportPath(const TArray<FVector>& Path);
	void UpdateTeleportArrow();
	void UpdateDestinationMarker();
	void ResetTeleport();

	// Default sub object
	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* MotionController = nullptr;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ControllerMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* GrabSphere = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UHapticFeedbackEffect_Curve* HandHoldRumble = nullptr;

	UPROPERTY(VisibleAnywhere)
	UPhysicsHandleComponent* PickupHandle = nullptr;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* PickupLocation = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float ThumbDeadZone = 0.25f;

	UPROPERTY(VisibleAnywhere)
	USplineComponent* TeleportPath = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TeleportDestinationMarker = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TeleportArrow = nullptr;

	UPROPERTY()
	TArray<USplineMeshComponent*> TeleportArcMeshPool;

	// Configuration properties
	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	float TeleportFadeDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	FVector TeleportProjectionExtent = FVector(100.f, 100.f, 20.f);

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	float TeleportProjectileSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	float TeleportSimulationTime = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	UStaticMesh* TeleportArcMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	UMaterialInterface* TeleportArcMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHandAnimInstance> HandAnimClass = nullptr;

	//State
	bool bCanClimb = false;
	bool bIsClimbing = false;
	FVector ClimbStartLocation;

	bool bCanCarry = false;
	bool bIsCarrying = false;

	bool bCanGrab = false;
	bool bIsGrabbing = false;

	AHandController* PairedController = nullptr;
	AActor* HeldActor = nullptr;

	bool bTeleportActive = false;
	bool bTeleportLocked = false;
	FRotator TeleportRotation;
	bool bHasTeleportDestination = false;
	float TeleportRightAxis = 0.f;
	float TeleportUpAxis = 0.f;
};

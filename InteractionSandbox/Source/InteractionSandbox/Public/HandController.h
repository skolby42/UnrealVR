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

UCLASS()
class INTERACTIONSANDBOX_API AHandController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandController();

	void SetHand(EControllerHand Hand);
	void PairController(AHandController* OtherController);
	bool FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation) const;
	bool IsHoldingComponent(UPrimitiveComponent* OtherComponent) const;
	bool IsHoldingActor(AActor* OtherActor) const;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateControllerMesh(EControllerHand Hand);

	UFUNCTION(BlueprintPure)
	UStaticMeshComponent* GetControllerMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Grip();
	void Release();
	
	float GetThumbDeadZone();
	void UpdateSkeletalMesh(EControllerHand Hand);
	void SetControllerMeshVisibility(bool IsVisible);
	void SetSkeletalMeshVisibility(bool IsVisible);

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

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	FVector TeleportProjectionExtent = FVector(100.f);

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	float TeleportProjectileSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleporting")
	float TeleportSimulationTime = 1.f;

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
};

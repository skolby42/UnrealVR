// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HandController.generated.h"

class UHapticFeedbackEffect_Curve;
class UMotionControllerComponent;
class UPhysicsHandleComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHandControllerDelegate);

UCLASS()
class ARCHITECTUREEXPLORER_API AHandController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandController();

	void SetHand(EControllerHand Hand);
	void PairController(AHandController* OtherController);
	bool FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation) const;
	bool IsCarrying(UPrimitiveComponent* OtherActor) const;

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
	void StartCarry();
	void StartClimb();
	void Release();
	void FinishCarry();
	void FinishClimb();
	float GetThumbDeadZone();

private:
	// Callbacks
	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Default sub object
	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* MotionController = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ControllerMesh = nullptr;

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

	// Helpers
	void ControllerRumble() const;
	AActor* GetOverlappingActorWithTag(const FName& Tag) const;
	bool CanClimb() const;
	void UpdateClimb();
	bool CanCarry() const;
	void UpdateCarry();

	//State
	bool bCanClimb = false;
	bool bIsClimbing = false;
	FVector ClimbStartLocation;

	bool bCanCarry = false;
	bool bIsCarrying = false;

	AHandController* PairedController;
};

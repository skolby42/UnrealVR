// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "Pistol.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIONSANDBOX_API APistol : public AGunBase
{
	GENERATED_BODY()

public:
	APistol();

	void Pickup(UPrimitiveComponent* AttachParent) override;
	void Drop() override;
	void InvertRotation() override;

	void FirePrimary() override;
	void ReleasePrimary() override;
	void Reload() override;
	
private:
	class UGunAnimInstance* GetGunAnimInstance();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* PistolSkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float LaunchVelocity = 3600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int AmmoCount = 10;
};

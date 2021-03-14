// Copyright Scott Kolby 2021

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
	void LaunchProjectile(FVector& SpawnLocation, FRotator& SpawnRotation);
	void ReleasePrimary() override;
	void Reload() override;

protected:
	void BeginPlay() override;
	
private:
	class UGunAnimInstance* GetGunAnimInstance();
	UFUNCTION()
	void ReloadComplete();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* PistolSkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	class UParticleSystem* MuzzleFlash = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float LaunchVelocity = 3600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int ClipSize = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundCue* EmptyFireSound;

	// State
	int ClipAmmoRemaining = 0;
};

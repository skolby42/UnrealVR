// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.h"
#include "GunBase.generated.h"

UCLASS()
class INTERACTIONSANDBOX_API AGunBase : public AActor, public IPickupActor
{
	GENERATED_BODY()
	
public:	
	AGunBase();

	virtual void Tick(float DeltaTime) override;
	virtual void FirePrimary() { }
	virtual void FireSecondary() { }
	virtual void Reload() { }
	
	virtual void Pickup(UPrimitiveComponent* AttachParent) override { }
	virtual void Drop() override { }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int AmmoCount = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float ReloadTime = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<class AProjectileBase> PrimaryProjectile = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<class AProjectileBase> SecondaryProjectile = nullptr;
};

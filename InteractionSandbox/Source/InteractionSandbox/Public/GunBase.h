// Copyright Scott Kolby 2021

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
	virtual void ReleasePrimary() { }
	virtual void FireSecondary() { }
	virtual void ReleaseSecondary() { }
	virtual void Reload() { }
	
	virtual void Pickup(UPrimitiveComponent* AttachParent) override { }
	virtual void Drop() override { }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<class AProjectile> PrimaryProjectile = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<class AProjectile> SecondaryProjectile = nullptr;
};

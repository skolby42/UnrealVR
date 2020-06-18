// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.h"
#include "PickupCube.generated.h"

UCLASS()
class INTERACTIONSANDBOX_API APickupCube : public AActor, public IPickupActor
{
	GENERATED_BODY()
	
public:	
	APickupCube();

protected:
	UFUNCTION(BlueprintPure)
	UStaticMeshComponent* GetStaticMeshComponent();

public:	
	UFUNCTION(BlueprintImplementableEvent)
	void Pickup(UPrimitiveComponent* AttachTo) override;

	UFUNCTION(BlueprintImplementableEvent)
	void Drop() override;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent = nullptr;
};

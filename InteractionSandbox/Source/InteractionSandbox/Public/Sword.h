// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.h"
#include "Sword.generated.h"

UCLASS()
class INTERACTIONSANDBOX_API ASword : public AActor, public IPickupActor
{
	GENERATED_BODY()
	
public:	
	ASword();

public:	
	void Pickup(UPrimitiveComponent* AttachParent) override;
	void Drop() override;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SwordMesh = nullptr;
};

// Copyright Scott Kolby 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UPickupActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INTERACTIONSANDBOX_API IPickupActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Pickup(UPrimitiveComponent* AttachParent) {}
	virtual void Drop() {}
	virtual void InvertRotation() {}
};

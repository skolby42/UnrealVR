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
	
private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* PistolSkeletalMesh = nullptr;
};

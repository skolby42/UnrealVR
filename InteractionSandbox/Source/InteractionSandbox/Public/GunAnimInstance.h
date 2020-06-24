// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GunAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTIONSANDBOX_API UGunAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void SetCanFire(bool Value);
	void SetReloading(bool Value);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bCanFire = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bReloading = false;
};

// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GunAnimInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGunAnimInstanceDelegate);

/**
 * 
 */
UCLASS()
class INTERACTIONSANDBOX_API UGunAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	bool GetCanFire() const;
	void SetCanFire(bool bNewCanFire);
	void SetReloading(bool bNewReloading);

	FGunAnimInstanceDelegate OnReloadComplete;

protected:
	UFUNCTION(BlueprintCallable)
	void ReloadComplete();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bCanFire = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bReloading = false;
};

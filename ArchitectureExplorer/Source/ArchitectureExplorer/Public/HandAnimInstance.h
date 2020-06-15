// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HandAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ARCHITECTUREEXPLORER_API UHandAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void SetCanGrab(bool Colliding);
	void SetGripHeld(bool Held);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool CanGrab = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool GripHeld = false;
};

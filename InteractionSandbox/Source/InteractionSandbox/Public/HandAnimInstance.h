// Copyright Scott Kolby 2021

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HandAnimInstance.generated.h"

UENUM(BlueprintType)
enum EGripType
{
	Default,
	Pistol
};

/**
 * 
 */
UCLASS()
class INTERACTIONSANDBOX_API UHandAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void SetCanGrab(bool NewCanGrab);
	void SetGripHeld(bool NewHeld);
	void SetGripType(EGripType NewGripType);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool CanGrab = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool GripHeld = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TEnumAsByte<EGripType> GripType = EGripType::Default;
};

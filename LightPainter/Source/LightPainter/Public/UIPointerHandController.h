// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "HandControllerBase.h"
#include "UIPointerHandController.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API AUIPointerHandController : public AHandControllerBase
{
	GENERATED_BODY()

public:
	AUIPointerHandController();

	void TriggerPressed() override;
	void TriggerReleased() override;

private:
	UPROPERTY(VisibleAnywhere)
	class UWidgetInteractionComponent* Pointer = nullptr;
};

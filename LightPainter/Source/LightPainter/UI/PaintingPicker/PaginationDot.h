// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaginationDot.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API UPaginationDot : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetActive(bool bActive);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(BindWidget))
	class UImage* DotImage;

private:
	UPROPERTY(EditDefaultsOnly)
	float InactiveOpacity = 0.4f;
};

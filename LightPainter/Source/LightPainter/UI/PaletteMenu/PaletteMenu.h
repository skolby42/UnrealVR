// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PaletteMenu.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API UPaletteMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	bool Initialize() override;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	UButton* ExitButton;

private:
	UFUNCTION()
	void ExitButtonPressed();
};

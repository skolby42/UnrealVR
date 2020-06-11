// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ActionBar.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTPAINTER_API UActionBar : public UUserWidget
{
	GENERATED_BODY()

public:
	bool Initialize() override;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	UButton* AddButton;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	UButton* DeleteButton;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
	UButton* ExitButton;

private:
	UFUNCTION()
	void AddButtonClicked();

	UFUNCTION()
	void DeleteButtonClicked();

	UFUNCTION()
	void ExitButtonClicked();
};

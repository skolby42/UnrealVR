// Copyright Scott Kolby 2020


#include "UIPointerHandController.h"
#include "Components/WidgetInteractionComponent.h"

AUIPointerHandController::AUIPointerHandController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Pointer = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteraction"));
	Pointer->SetupAttachment(GetRootComponent());
}

void AUIPointerHandController::TriggerPressed()
{
	if (Pointer)
	{
		Pointer->PressPointerKey(EKeys::LeftMouseButton);
	}
}

void AUIPointerHandController::TriggerReleased()
{
	if (Pointer)
	{
		Pointer->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

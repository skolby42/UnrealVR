// Copyright Scott Kolby 2020


#include "WidgetTouchingComponent.h"

void UWidgetTouchingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsOverInteractableWidget() && !bIsPointerKeyPressed)
	{
		PressPointerKey(EKeys::LeftMouseButton);
		bIsPointerKeyPressed = true;
	}

	if (!IsOverInteractableWidget() && bIsPointerKeyPressed)
	{
		ReleasePointerKey(EKeys::LeftMouseButton);
		bIsPointerKeyPressed = false;
	}
}
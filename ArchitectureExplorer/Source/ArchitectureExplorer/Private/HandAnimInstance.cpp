// Copyright Scott Kolby 2020


#include "HandAnimInstance.h"

void UHandAnimInstance::SetCanGrab(bool Colliding)
{
	CanGrab = Colliding;
}

void UHandAnimInstance::SetGripHeld(bool Held)
{
	GripHeld = Held;
}

// Copyright Scott Kolby 2020


#include "HandAnimInstance.h"

void UHandAnimInstance::SetCanGrab(bool NewCanGrab)
{
	CanGrab = NewCanGrab;
}

void UHandAnimInstance::SetGripHeld(bool NewHeld)
{
	GripHeld = NewHeld;
}

void UHandAnimInstance::SetGripType(EGripType NewGripType)
{
	GripType = NewGripType;
}

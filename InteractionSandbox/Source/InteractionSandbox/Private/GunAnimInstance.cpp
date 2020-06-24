// Copyright Scott Kolby 2020


#include "GunAnimInstance.h"

void UGunAnimInstance::SetCanFire(bool Value)
{
	bCanFire = Value;
}

void UGunAnimInstance::SetReloading(bool Value)
{
	bReloading = Value;

	if (bReloading) bCanFire = false;
}

// Copyright Scott Kolby 2021


#include "GunAnimInstance.h"

bool UGunAnimInstance::GetCanFire() const
{
	return bCanFire;
}

void UGunAnimInstance::SetCanFire(bool bNewCanFire)
{
	bCanFire = bNewCanFire;
}

void UGunAnimInstance::SetReloading(bool bNewReloading)
{
	bReloading = bNewReloading;
	bCanFire = !bReloading;
}

void UGunAnimInstance::ReloadComplete()
{
	SetReloading(false);
	OnReloadComplete.Broadcast();
}

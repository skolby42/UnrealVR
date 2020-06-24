// Copyright Scott Kolby 2020


#include "Pistol.h"
#include "Components/SkeletalMeshComponent.h"
#include "GunAnimInstance.h"
#include "Projectile.h"

APistol::APistol()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PistolSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	PistolSkeletalMesh->SetupAttachment(Root);
}

void APistol::Pickup(UPrimitiveComponent* AttachParent)
{
	if (!PistolSkeletalMesh || !AttachParent) return;

	PistolSkeletalMesh->SetSimulatePhysics(false);
	PistolSkeletalMesh->AttachToComponent(AttachParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
}

void APistol::Drop()
{
	if (!PistolSkeletalMesh) return;

	PistolSkeletalMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	PistolSkeletalMesh->SetSimulatePhysics(true);
}

void APistol::InvertRotation()
{
	FRotator Rotation = PistolSkeletalMesh->GetRelativeRotation();
	Rotation.Roll += 180;
	Rotation.Yaw += 180;
	PistolSkeletalMesh->SetRelativeRotation(Rotation);
}

void APistol::FirePrimary()
{
	UGunAnimInstance* AnimInstance = GetGunAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->SetCanFire(true);
	}

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
		PrimaryProjectile, 
		PistolSkeletalMesh->GetSocketLocation(TEXT("MuzzleFlash")), 
		PistolSkeletalMesh->GetSocketRotation(TEXT("MuzzleFlash")));
	
	if (!Projectile) return;
	Projectile->Launch(LaunchVelocity);
}

void APistol::ReleasePrimary()
{
	UGunAnimInstance* AnimInstance = GetGunAnimInstance();
	if (!AnimInstance) return;

	AnimInstance->SetCanFire(false);
}

void APistol::Reload()
{
	UGunAnimInstance* AnimInstance = GetGunAnimInstance();
	AnimInstance->SetReloading(true);
}

UGunAnimInstance* APistol::GetGunAnimInstance()
{
	if (!PistolSkeletalMesh) return nullptr;
	return Cast<UGunAnimInstance>(PistolSkeletalMesh->GetAnimInstance());
}

// Copyright Scott Kolby 2021


#include "Pistol.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "GunAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "Sound/SoundCue.h"

APistol::APistol()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PistolSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	PistolSkeletalMesh->SetupAttachment(Root);
}

void APistol::BeginPlay()
{
	Super::BeginPlay();

	ClipAmmoRemaining = ClipSize;
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

	FVector SpawnLocation = PistolSkeletalMesh->GetSocketLocation(TEXT("MuzzleFlash"));
	FRotator SpawnRotation = PistolSkeletalMesh->GetSocketRotation(TEXT("MuzzleFlash"));

	if (ClipAmmoRemaining <= 0)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptyFireSound, SpawnLocation);
	}
	else
	{
		LaunchProjectile(SpawnLocation, SpawnRotation);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SpawnLocation);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, SpawnLocation);
	}
}

void APistol::LaunchProjectile(FVector& SpawnLocation, FRotator& SpawnRotation)
{
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(PrimaryProjectile, SpawnLocation, SpawnRotation);
	if (Projectile)
		Projectile->Launch(LaunchVelocity);

	ClipAmmoRemaining--;
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
	if (AnimInstance)
	{
		AnimInstance->SetReloading(true);
		AnimInstance->OnReloadComplete.AddDynamic(this, &APistol::ReloadComplete);
	}
}

void APistol::ReloadComplete()
{
	ClipAmmoRemaining = ClipSize;

	UGunAnimInstance* AnimInstance = GetGunAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnReloadComplete.RemoveDynamic(this, &APistol::ReloadComplete);
	}
}

UGunAnimInstance* APistol::GetGunAnimInstance()
{
	if (!PistolSkeletalMesh) return nullptr;
	return Cast<UGunAnimInstance>(PistolSkeletalMesh->GetAnimInstance());
}



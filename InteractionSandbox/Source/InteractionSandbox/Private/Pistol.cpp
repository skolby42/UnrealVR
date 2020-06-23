// Copyright Scott Kolby 2020


#include "Pistol.h"
#include "Components/SkeletalMeshComponent.h"

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
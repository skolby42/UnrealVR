// Copyright Scott Kolby 2020


#include "Sword.h"
#include "Components/StaticMeshComponent.h"


ASword::ASword()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	SwordMesh->SetupAttachment(Root);
}

void ASword::Pickup(UPrimitiveComponent* AttachParent)
{
	if (!SwordMesh || !AttachParent) return;

	SwordMesh->SetSimulatePhysics(false);
	SwordMesh->AttachToComponent(AttachParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
}

void ASword::Drop()
{
	if (!SwordMesh) return;

	SwordMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	SwordMesh->SetSimulatePhysics(true);
}

void ASword::InvertRotation()
{
	FRotator Rotation = SwordMesh->GetRelativeRotation();
	Rotation.Roll += 180;
	Rotation.Yaw += 180;
	SwordMesh->SetRelativeRotation(Rotation);
}


// Copyright Scott Kolby 2021


#include "Dowel.h"
#include "AttachPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"


ADowel::ADowel()
{
	PrimaryActorTick.bCanEverTick = false;

	DowelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DowelMesh"));
	SetRootComponent(DowelMesh);

	PreviewDowelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewDowelMesh"));
	PreviewDowelMesh->SetVisibility(false);
	PreviewDowelMesh->SetupAttachment(GetRootComponent());
}

void ADowel::Pickup(UPrimitiveComponent* AttachParent)
{
	if (!DowelMesh || !AttachParent) return;

	AAttachPoint* AttachPoint = GetOverlappingAttachPoint();
	if (AttachPoint)
	{
		AttachPoint->ReleaseComponent();
	}

	DowelMesh->SetSimulatePhysics(false);
	DowelMesh->AttachToComponent(AttachParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("CarrySocket"));
}

void ADowel::Drop()
{
	if (!DowelMesh) return;

	DowelMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	AAttachPoint* AttachPoint = GetOverlappingAttachPoint();
	if (AttachPoint)
	{
		AttachPoint->AttachComponent(DowelMesh);

		if (PreviewDowelMesh)
		{
			PreviewDowelMesh->SetVisibility(false);
		}
	}
	else
	{
		DowelMesh->SetSimulatePhysics(true);
	}
}

void ADowel::AttachPreviewMesh(UPrimitiveComponent* Parent)
{
	if (!PreviewDowelMesh) return;
	PreviewDowelMesh->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("AttachSocket"));
	PreviewDowelMesh->SetVisibility(true);
}

void ADowel::DetatchPreviewMesh()
{
	if (!PreviewDowelMesh) return;
	PreviewDowelMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	PreviewDowelMesh->SetVisibility(false);
}

AAttachPoint* ADowel::GetOverlappingAttachPoint() const
{
	TSet<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AAttachPoint::StaticClass());
	for (AActor* Actor : OverlappingActors)
	{
		return Cast<AAttachPoint>(Actor);
	}

	return nullptr;
}


// Copyright Scott Kolby 2020


#include "AttachPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Dowel.h"


AAttachPoint::AAttachPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	AttachPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachPointMesh"));
	SetRootComponent(AttachPointMesh);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

void AAttachPoint::BeginPlay()
{
	Super::BeginPlay();
	
	if (Sphere)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAttachPoint::SphereBeginComponentOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &AAttachPoint::SphereEndComponentOverlap);
	}
}

void AAttachPoint::SphereBeginComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADowel* Dowel = Cast<ADowel>(OtherActor);
	if (!Dowel) return;

	Dowel->AttachPreviewMesh(AttachPointMesh);
}

void AAttachPoint::SphereEndComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADowel* Dowel = Cast<ADowel>(OtherActor);
	if (!Dowel) return;

	Dowel->DetatchPreviewMesh();
}

bool AAttachPoint::AttachComponent(UPrimitiveComponent* Component)
{
	if (AttachedComponent) return false;

	Component->AttachToComponent(AttachPointMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("AttachSocket"));
	Component->SetSimulatePhysics(false);

	AttachedComponent = Component;
	return true;
}

void AAttachPoint::ReleaseComponent()
{
	AttachedComponent = nullptr;
}


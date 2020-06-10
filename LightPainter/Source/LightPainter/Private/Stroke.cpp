// Copyright Scott Kolby 2020


#include "Stroke.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AStroke::AStroke()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	StrokeMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMesh"));
	StrokeMeshes->SetupAttachment(Root);
}

void AStroke::Update(const FVector& CursorLocation)
{	
	if (StrokeMeshes)
	{
		StrokeMeshes->AddInstance(GetNextSegmentTransform(CursorLocation));
	}

	PrevCursorLocation = CursorLocation;
}

void AStroke::InitializeStartPoint()
{
	PrevCursorLocation = GetActorLocation();
}

FTransform AStroke::GetNextSegmentTransform(const FVector CurrentLocation) const
{
	FTransform SegmentTransform;

	SegmentTransform.SetLocation(GetNextSegmentLocation());
	SegmentTransform.SetRotation(GetNextSegmentRotation(CurrentLocation));
	SegmentTransform.SetScale3D(GetNextSegmentScale(CurrentLocation));

	return SegmentTransform;
}

FVector AStroke::GetNextSegmentLocation() const
{
	return GetTransform().InverseTransformPosition(PrevCursorLocation);  // Convert from world to local coordinates
}

FQuat AStroke::GetNextSegmentRotation(const FVector CurrentLocation) const
{
	FVector Segment = CurrentLocation - PrevCursorLocation;
	return FQuat::FindBetweenNormals(GetActorForwardVector(), Segment.GetSafeNormal());
}

FVector AStroke::GetNextSegmentScale(const FVector CurrentLocation) const
{
	FVector Segment = CurrentLocation - PrevCursorLocation;
	return FVector(Segment.Size(), 1.f, 1.f);
}

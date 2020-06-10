// Copyright Scott Kolby 2020


#include "Stroke.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AStroke::AStroke()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	StrokeMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StrokeMeshes"));
	StrokeMeshes->SetupAttachment(Root);

	JointMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StrokeJoints"));
	JointMeshes->SetupAttachment(Root);
}

void AStroke::Update(const FVector& CursorLocation)
{
	if (InitializePrevLocation(CursorLocation)) return;

	if (StrokeMeshes)
	{
		StrokeMeshes->AddInstance(GetNextSegmentTransform(CursorLocation));
	}

	if (JointMeshes)
	{
		JointMeshes->AddInstance(GetNextJointTransform(CursorLocation));
	}

	PrevCursorLocation = CursorLocation;
}

bool AStroke::InitializePrevLocation(const FVector& CurrentLocation)
{
	if (PrevCursorLocation.IsNearlyZero()) {

		PrevCursorLocation = CurrentLocation;
		if (JointMeshes)
		{
			JointMeshes->AddInstance(GetNextJointTransform(CurrentLocation));
		}
		return true;
	}

	return false;
}

FTransform AStroke::GetNextSegmentTransform(const FVector& CurrentLocation) const
{
	FTransform SegmentTransform;

	SegmentTransform.SetLocation(GetNextSegmentLocation(PrevCursorLocation));
	SegmentTransform.SetRotation(GetNextSegmentRotation(CurrentLocation));
	SegmentTransform.SetScale3D(GetNextSegmentScale(CurrentLocation));

	return SegmentTransform;
}

FTransform AStroke::GetNextJointTransform(const FVector& CurrentLocation) const
{
	FTransform JointTransform;
	JointTransform.SetLocation(GetNextSegmentLocation(CurrentLocation));
	return JointTransform;
}

FVector AStroke::GetNextSegmentLocation(const FVector& CurrentLocation) const
{
	return GetTransform().InverseTransformPosition(CurrentLocation);  // Convert from world to local coordinates
}

FQuat AStroke::GetNextSegmentRotation(const FVector& CurrentLocation) const
{
	FVector Segment = CurrentLocation - PrevCursorLocation;
	return FQuat::FindBetweenNormals(GetActorForwardVector(), Segment.GetSafeNormal());
}

FVector AStroke::GetNextSegmentScale(const FVector& CurrentLocation) const
{
	FVector Segment = CurrentLocation - PrevCursorLocation;
	return FVector(Segment.Size(), 1.f, 1.f);
}

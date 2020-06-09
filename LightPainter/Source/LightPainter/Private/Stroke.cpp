// Copyright Scott Kolby 2020


#include "Stroke.h"
#include "Components/SplineMeshComponent.h"

// Sets default values
AStroke::AStroke()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void AStroke::Update(const FVector& CursorLocation)
{	
	USplineMeshComponent* Spline = CreateSplineMesh(CursorLocation);

	FVector StartLocation = GetActorTransform().InverseTransformPosition(PrevCursorLocation);  // Convert from world to local coordinates
	FVector EndLocation = GetActorTransform().InverseTransformPosition(CursorLocation);
	FVector StartTangent, EndTangent = FVector::ZeroVector;
	/*StrokePath->GetLocalLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent);
	StrokePath->GetLocalLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent);*/

	Spline->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);

	PrevCursorLocation = CursorLocation;
}

USplineMeshComponent* AStroke::CreateSplineMesh(const FVector& CursorLocation)
{
	USplineMeshComponent* NewSpline = NewObject<USplineMeshComponent>(this);

	NewSpline->SetMobility(EComponentMobility::Movable);
	NewSpline->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetIncludingScale);

	if (SplineMesh)
		NewSpline->SetStaticMesh(SplineMesh);

	if (SplineMaterial)
		NewSpline->SetMaterial(0, SplineMaterial);

	NewSpline->RegisterComponent();  // Important for dynamic components

	return NewSpline;
}

void AStroke::InitializeStartPoint()
{
	PrevCursorLocation = GetActorLocation();
}
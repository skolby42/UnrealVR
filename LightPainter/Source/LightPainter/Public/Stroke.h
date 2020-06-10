// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightPainter/Saving/LightPainterSaveGame.h"
#include "Stroke.generated.h"

UCLASS()
class LIGHTPAINTER_API AStroke : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStroke();
	
	void Update(const FVector& CursorLocation);
	FStrokeState SerializeToStruct() const;
	static AStroke* DeserializeFromStruct(UWorld* World, const FStrokeState& StrokeState);

private:
	bool InitializePrevLocation(const FVector& CurrentLocation);
	FTransform GetNextSegmentTransform(const FVector& CurrentLocation) const;
	FTransform GetNextJointTransform(const FVector& CurrentLocation) const;

	FVector GetNextSegmentLocation(const FVector& CurrentLocation) const;
	FQuat GetNextSegmentRotation(const FVector& CurrentLocation) const;
	FVector GetNextSegmentScale(const FVector& CurrentLocation) const;


	// Components
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UInstancedStaticMeshComponent* StrokeMeshes = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UInstancedStaticMeshComponent* JointMeshes = nullptr;

	//State
	FVector PrevCursorLocation;
	TArray<FVector> ControlPoints;
};

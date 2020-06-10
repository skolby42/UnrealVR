// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Stroke.generated.h"

UCLASS()
class LIGHTPAINTER_API AStroke : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStroke();
	
	void Update(const FVector& CursorLocation);
	void InitializeStartPoint();

private:
	FTransform GetNextSegmentTransform(const FVector CurrentLocation) const;
	FVector GetNextSegmentLocation() const;
	FQuat GetNextSegmentRotation(const FVector CurrentLocation) const;
	FVector GetNextSegmentScale(const FVector CurrentLocation) const;


	// Components
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UInstancedStaticMeshComponent* StrokeMeshes = nullptr;

	//State
	FVector PrevCursorLocation;
};

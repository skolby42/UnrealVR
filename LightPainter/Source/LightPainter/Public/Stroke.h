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
	class USplineMeshComponent* CreateSplineMesh(const FVector& CursorLocation);

private:
	// Components
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	// Config
	UPROPERTY(EditAnywhere, Category = "Setup")
	UStaticMesh* SplineMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setup")
	UMaterialInterface* SplineMaterial = nullptr;

	//State
	FVector PrevCursorLocation;
};

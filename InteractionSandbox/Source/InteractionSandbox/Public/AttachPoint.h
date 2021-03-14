// Copyright Scott Kolby 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttachPoint.generated.h"

UCLASS()
class INTERACTIONSANDBOX_API AAttachPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AAttachPoint();
	bool AttachComponent(UPrimitiveComponent* Component);
	void ReleaseComponent();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void SphereBeginComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void SphereEndComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachPointMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Sphere = nullptr;

	// State
	UPrimitiveComponent* AttachedComponent = nullptr;
};

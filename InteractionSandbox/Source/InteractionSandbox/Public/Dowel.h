// Copyright Scott Kolby 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.h"
#include "Dowel.generated.h"

UCLASS()
class INTERACTIONSANDBOX_API ADowel : public AActor, public IPickupActor
{
	GENERATED_BODY()
	
public:	
	ADowel();

	void Pickup(UPrimitiveComponent* AttachParent) override;
	void Drop() override;
	
	void AttachPreviewMesh(UPrimitiveComponent* Parent);
	void DetatchPreviewMesh();

private:
	class AAttachPoint* GetOverlappingAttachPoint() const;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DowelMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PreviewDowelMesh = nullptr;
};

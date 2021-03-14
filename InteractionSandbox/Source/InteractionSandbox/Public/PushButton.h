// Copyright Scott Kolby 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PushButton.generated.h"

UCLASS()
class INTERACTIONSANDBOX_API APushButton : public AActor
{
	GENERATED_BODY()
	
public:	
	APushButton();

	void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnFaceBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnFaceEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FVector GetOverlapVerticalOffset();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Base = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Face = nullptr;

	// State
	FVector StartLocation;
	FVector StartOffset;
};

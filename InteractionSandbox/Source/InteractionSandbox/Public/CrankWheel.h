// Copyright Scott Kolby 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrankWheel.generated.h"

UCLASS()
class INTERACTIONSANDBOX_API ACrankWheel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrankWheel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Base = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Wheel = nullptr;
};

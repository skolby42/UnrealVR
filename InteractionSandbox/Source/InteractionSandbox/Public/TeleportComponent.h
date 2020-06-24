// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeleportComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTIONSANDBOX_API UTeleportComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTeleportComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	class USplineComponent* TeleportPath = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DestinationMarker = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TeleportArrow = nullptr;
};

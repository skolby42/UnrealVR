// Copyright Scott Kolby 2021


#include "PickupCube.h"
#include "Components/StaticMeshComponent.h"


APickupCube::APickupCube()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(Root);
}

UStaticMeshComponent* APickupCube::GetStaticMeshComponent()
{
	return StaticMeshComponent;
}

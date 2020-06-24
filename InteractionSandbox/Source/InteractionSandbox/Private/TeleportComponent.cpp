// Copyright Scott Kolby 2020


#include "TeleportComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UTeleportComponent::UTeleportComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetCollisionProfileName(TEXT("NoCollision"));
	//DestinationMarker->SetupAttachment(GetRootComponent());

	TeleportArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportArrow"));
	TeleportArrow->SetCollisionProfileName(TEXT("NoCollision"));
	//TeleportArrow->SetupAttachment(DestinationMarker);
}


// Called when the game starts
void UTeleportComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTeleportComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


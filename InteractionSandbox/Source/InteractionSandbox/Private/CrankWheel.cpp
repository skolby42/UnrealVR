// Copyright Scott Kolby 2021


#include "CrankWheel.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACrankWheel::ACrankWheel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	Base->SetupAttachment(Root);

	Wheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel"));
	Wheel->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ACrankWheel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACrankWheel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


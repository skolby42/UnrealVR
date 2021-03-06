// Copyright Scott Kolby 2020


#include "HandControllerBase.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"

// Sets default values
AHandControllerBase::AHandControllerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);
	//MotionController->SetShowDeviceModel(true);

	ControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControllerMesh"));
	ControllerMesh->SetupAttachment(MotionController);
}

// Called when the game starts or when spawned
void AHandControllerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

AHandControllerBase* AHandControllerBase::GetPairedController()
{
	return PairedController;
}

// Called every frame
void AHandControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHandControllerBase::SetHand(EControllerHand ControllerHand)
{
	if (!MotionController) return;
	MotionController->SetTrackingSource(ControllerHand);
	UpdateControllerMesh(ControllerHand);
}

void AHandControllerBase::Pair(AHandControllerBase* OtherController)
{
	PairedController = OtherController;
	OtherController->PairedController = this;
}

UStaticMeshComponent* AHandControllerBase::GetControllerMesh()
{
	return ControllerMesh;
}

// Copyright Scott Kolby 2020


#include "HandController.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "MotionControllerComponent.h"


// Sets default values
AHandController::AHandController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);

	ControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControllerMesh"));
	ControllerMesh->SetupAttachment(MotionController);
}

UStaticMeshComponent* AHandController::GetControllerMesh()
{
	return ControllerMesh;
}

// Called when the game starts or when spawned
void AHandController::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &AHandController::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AHandController::ActorEndOverlap);
}

// Called every frame
void AHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Climb();
}

void AHandController::SetHand(EControllerHand ControllerHand)
{
	if (!MotionController) return;
	MotionController->SetTrackingSource(ControllerHand);
	UpdateControllerMesh(ControllerHand);
}

void AHandController::PairController(AHandController* OtherController)
{
	PairedController = OtherController;
	OtherController->PairedController = this;
}

void AHandController::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!bCanClimb && CanClimb())
	{
		bCanClimb = true;
		ControllerRumble();
	}
}

void AHandController::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanClimb = CanClimb();
}

bool AHandController::CanClimb() const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->ActorHasTag(TEXT("Climbable"))) return true;
	}

	return false;
}

void AHandController::ControllerRumble() const
{
	if (!HandHoldRumble) return;

	APawn* Pawn = Cast<APawn>(GetAttachParentActor());
	if (!Pawn) return;

	APlayerController* PlayerController = Pawn->GetController<APlayerController>();
	if (!PlayerController) return;

	PlayerController->PlayHapticEffect(HandHoldRumble, MotionController->GetTrackingSource());
}

void AHandController::Grip()
{
	if (bCanClimb && !bIsClimbing)
	{
		bIsClimbing = true;
		ClimbingStartLocation = GetActorLocation();

		if (PairedController && PairedController->bIsClimbing)
			PairedController->Release();

		ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
		if (Character)
		{
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		}
	}
}

void AHandController::Release()
{
	if (bIsClimbing)
	{
		bIsClimbing = false;

		ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
		if (Character)
		{
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		}
	}
}

void AHandController::Climb()
{
	if (!bIsClimbing) return;

	AActor* Parent = GetAttachParentActor();
	if (!Parent) return;
	
	FVector ControllerDelta = GetActorLocation() - ClimbingStartLocation;
	AddActorWorldOffset(ControllerDelta);
	Parent->AddActorWorldOffset(-ControllerDelta);
}

// Copyright Scott Kolby 2020


#include "HandController.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "DrawDebugHelpers.h"


// Sets default values
AHandController::AHandController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);

	ControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControllerMesh"));
	ControllerMesh->SetupAttachment(MotionController);

	PickupHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PickupHandle"));

	PickupLocation = CreateDefaultSubobject<USceneComponent>(TEXT("PickupLocation"));
	PickupLocation->SetupAttachment(MotionController);
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

	UpdateClimb();
	UpdateCarry();
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

bool AHandController::FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation) const
{
	FVector LookVector = GetActorForwardVector();
	FVector Start = GetActorLocation() + LookVector * 4.f;  // Start line trace in front of motion controller
	FVector LaunchVector = LookVector * TeleportProjectileSpeed;

	FPredictProjectilePathParams ProjectileParams(
		0.f,
		Start,
		LaunchVector,
		TeleportSimulationTime,
		ECollisionChannel::ECC_Visibility);
	//ProjectileParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	ProjectileParams.bTraceComplex = true;  // Get more collision locations if needed

	FPredictProjectilePathResult ProjectileResult;

	bool bHit = UGameplayStatics::PredictProjectilePath(this, ProjectileParams, ProjectileResult);

	if (!bHit) return false;

	FNavLocation NavLocation;

	for (FPredictProjectilePathPointData PointData : ProjectileResult.PathData)
	{
		OutPath.Add(PointData.Location);
	}

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	bool bOnNavMesh = NavSystem->ProjectPointToNavigation(ProjectileResult.HitResult.Location, NavLocation, TeleportProjectionExtent);

	if (!bOnNavMesh) return false;

	OutLocation = ProjectileResult.HitResult.Location;

	return true;
}

void AHandController::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!bCanClimb && CanClimb())
	{
		bCanClimb = true;
		ControllerRumble();
	}

	if (!bCanCarry && CanCarry())
	{
		bCanCarry = true;
		ControllerRumble();
	}
}

void AHandController::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanClimb = CanClimb();
	bCanCarry = CanCarry();
}

bool AHandController::CanClimb() const
{
	AActor* Actor = GetOverlappingActorWithTag(TEXT("Climbable"));
	return Actor != nullptr;
}

void AHandController::UpdateClimb()
{
	if (!bIsClimbing) return;

	AActor* Parent = GetAttachParentActor();
	if (!Parent) return;

	FVector ControllerDelta = GetActorLocation() - ClimbStartLocation;
	AddActorWorldOffset(ControllerDelta);
	Parent->AddActorWorldOffset(-ControllerDelta);
}

bool AHandController::CanCarry() const
{
	AActor* Actor = GetOverlappingActorWithTag(TEXT("Carry"));
	return Actor != nullptr;
}

AActor* AHandController::GetOverlappingActorWithTag(const FName& Tag) const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->ActorHasTag(Tag)) return Actor;
	}

	return nullptr;
}

void AHandController::UpdateCarry()
{
	if (PickupLocation)
		PickupHandle->SetTargetLocation(PickupLocation->GetComponentLocation());
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
	StartClimb();
	StartCarry();
}

void AHandController::Release()
{
	FinishClimb();
	FinishCarry();
}

void AHandController::StartClimb()
{
	if (bCanClimb && !bIsClimbing)
	{
		bIsClimbing = true;
		ClimbStartLocation = GetActorLocation();

		if (PairedController && PairedController->bIsClimbing)
			PairedController->Release();

		ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
		if (Character)
		{
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		}
	}
}

void AHandController::FinishClimb()
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

void AHandController::StartCarry()
{
	if (!bIsCarrying && bCanCarry)
	{
		AActor* Actor = GetOverlappingActorWithTag(TEXT("Carry"));
		if (Actor)
		{
			UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
			if (Component)
			{
				bIsCarrying = true;
				PickupHandle->GrabComponentAtLocationWithRotation(Component, NAME_None, Component->GetComponentLocation(), Component->GetComponentRotation());
			}
		}
	}

	if (PairedController && PairedController->bIsCarrying)
		PairedController->Release();
}

void AHandController::FinishCarry()
{
	if (bIsCarrying && PickupHandle->GrabbedComponent)
	{
		PickupHandle->ReleaseComponent();
		bIsCarrying = false;
	}
}

float AHandController::GetThumbDeadZone()
{
	return ThumbDeadZone;
}

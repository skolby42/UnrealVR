// Copyright Scott Kolby 2020


#include "HandController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "HandAnimInstance.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "DrawDebugHelpers.h"


// Sets default values
AHandController::AHandController()
{
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);

	ControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControllerMesh"));
	ControllerMesh->SetupAttachment(MotionController);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetWorldRotation(FRotator(0.f, 0.f, 90.f));
	SkeletalMesh->SetWorldLocation(FVector(-9.f, 0.5f, -5.f));
	if (HandAnimClass)
	{
		SkeletalMesh->SetAnimClass(HandAnimClass);
		SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
	SkeletalMesh->SetupAttachment(MotionController);

	GrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphere"));
	GrabSphere->SetupAttachment(MotionController);

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

void AHandController::SetHand(EControllerHand Hand)
{
	if (!MotionController) return;
	MotionController->SetTrackingSource(Hand);
	UpdateControllerMesh(Hand);
	UpdateSkeletalMesh(Hand);
}

void AHandController::UpdateSkeletalMesh(EControllerHand Hand)
{
	if (Hand != EControllerHand::Left || !SkeletalMesh) return;
	SkeletalMesh->SetWorldScale3D(FVector(1.f, 1.f, -1.f));  // Invert scale for left hand
}

void AHandController::SetControllerMeshVisibility(bool IsVisible)
{
	if (!ControllerMesh) return;
	ControllerMesh->SetVisibility(IsVisible);
}

void AHandController::SetSkeletalMeshVisibility(bool IsVisible)
{
	if (!SkeletalMesh) return;
	SkeletalMesh->SetVisibility(IsVisible);
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

bool AHandController::IsCarrying(UPrimitiveComponent* OtherActor) const
{
	return PickupHandle && PickupHandle->GrabbedComponent == OtherActor;
}

void AHandController::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!bCanClimb && CanClimb())
	{
		bCanClimb = true;
		UpdateCanGrabAnim(true);
		ControllerRumble();
	}

	if (!bCanCarry && CanCarry())
	{
		bCanCarry = true;
		UpdateCanGrabAnim(true);
		ControllerRumble();
	}
}

void AHandController::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanClimb = CanClimb();
	bCanCarry = CanCarry();

	if (!bCanClimb && !bCanCarry)
	{
		UpdateCanGrabAnim(false);
	}
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

UHandAnimInstance* AHandController::GetHandAnimInstance() const
{
	if (!SkeletalMesh) return nullptr;
	return Cast<UHandAnimInstance>(SkeletalMesh->GetAnimInstance());
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
	UE_LOG(LogTemp, Warning, TEXT("Grip"))
	UpdateGripHeldAnim(true);
	StartClimb();
	StartCarry();
}

void AHandController::UpdateGripHeldAnim(bool GripHeld)
{
	UHandAnimInstance* HandAnimInstance = GetHandAnimInstance();
	if (!HandAnimInstance) return;

	HandAnimInstance->SetGripHeld(GripHeld);
}

void AHandController::UpdateCanGrabAnim(bool CanGrab)
{
	UHandAnimInstance* HandAnimInstance = GetHandAnimInstance();
	if (!HandAnimInstance) return;
	
	HandAnimInstance->SetCanGrab(CanGrab);
}

void AHandController::Release()
{
	UpdateGripHeldAnim(false);
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

				GrabComponent(Component);

				if (PairedController && PairedController->IsCarrying(Component))
					PairedController->Release();
			}
		}
	}
}

void AHandController::UpdateCarry()
{
	if (!PickupHandle || !PickupLocation) return;

	PickupHandle->SetTargetLocation(PickupLocation->GetComponentLocation());
	PickupHandle->SetTargetRotation(PickupLocation->GetComponentRotation());
}

void AHandController::FinishCarry()
{
	if (bIsCarrying && PickupHandle->GrabbedComponent)
	{
		PickupHandle->ReleaseComponent();
		bIsCarrying = false;
	}
}

void AHandController::GrabComponent(UPrimitiveComponent* Component)
{
	if (!Component || !PickupLocation || !PickupHandle || !GrabSphere) return;

	FVector Location = GrabSphere->GetComponentLocation() - Component->GetComponentLocation();
	FRotator Rotation = GrabSphere->GetComponentRotation() - Component->GetComponentRotation();

	PickupLocation->SetRelativeLocation(Location);
	PickupLocation->SetRelativeRotation(Rotation);

	PickupHandle->GrabComponentAtLocationWithRotation(Component, NAME_None, Component->GetComponentLocation(), Component->GetComponentRotation());
	//PickupHandle->GrabComponentAtLocation(Component, NAME_None, Component->GetComponentLocation());
}

float AHandController::GetThumbDeadZone()
{
	return ThumbDeadZone;
}

// Copyright Scott Kolby 2020


#include "HandController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GunBase.h"
#include "HandAnimInstance.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "PickupActor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "DrawDebugHelpers.h"

#include "Engine/SkeletalMeshSocket.h"


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

	TeleportDestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportDestinationMarker"));
	TeleportDestinationMarker->SetCollisionProfileName(TEXT("NoCollision"));
	TeleportDestinationMarker->SetupAttachment(GetRootComponent());

	TeleportArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportArrow"));
	TeleportArrow->SetCollisionProfileName(TEXT("NoCollision"));
	TeleportArrow->SetupAttachment(TeleportDestinationMarker);

	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportPath"));
	TeleportPath->SetupAttachment(GetRootComponent());
}

void AHandController::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &AHandController::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AHandController::ActorEndOverlap);

	if (TeleportDestinationMarker)
		TeleportDestinationMarker->SetVisibility(false);
}

void AHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateClimb();
	UpdateCarry();
	UpdateGrab();
	UpdateDestinationMarker();
}

UStaticMeshComponent* AHandController::GetControllerMesh()
{
	return ControllerMesh;
}

float AHandController::GetThumbDeadZone()
{
	return ThumbDeadZone;
}

bool AHandController::IsTeleportActive()
{
	return bTeleportActive;
}

void AHandController::SetHand(EControllerHand Hand)
{
	if (!MotionController) return;
	MotionController->SetTrackingSource(Hand);
	UpdateControllerMesh(Hand);
	UpdateSkeletalMesh(Hand);
	UpdateGrabSphere(Hand);
}

void AHandController::UpdateSkeletalMesh(EControllerHand Hand)
{
	if (Hand != EControllerHand::Left || !SkeletalMesh) return;

	// Invert scale for left hand
	FVector Scale = SkeletalMesh->GetComponentScale();
	Scale.Z *= -1;
	SkeletalMesh->SetWorldScale3D(Scale);
}

void AHandController::UpdateGrabSphere(EControllerHand Hand)
{
	if (Hand != EControllerHand::Left || !GrabSphere) return;

	// Invert location for left hand
	FVector Location = GrabSphere->GetComponentLocation();
	Location.Y *= -1;
	GrabSphere->SetWorldLocation(Location);
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
	
	TArray<AActor*> IgnoredActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), IgnoredActors);
	ProjectileParams.ActorsToIgnore = IgnoredActors;

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

bool AHandController::IsHoldingComponent(UPrimitiveComponent* OtherComponent) const
{
	return (PickupHandle && PickupHandle->GrabbedComponent == OtherComponent);
}

bool AHandController::IsHoldingActor(AActor* OtherActor) const
{
	return (HeldActor && Cast<AActor>(HeldActor) == OtherActor);
}

bool AHandController::IsHoldingObject() const
{
	return (PickupHandle && PickupHandle->GrabbedComponent != nullptr) || HeldActor != nullptr;
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

	if (!bCanGrab && CanGrab())
	{
		bCanGrab = true;
		UpdateCanGrabAnim(true);
		ControllerRumble();
	}
}

void AHandController::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanClimb = CanClimb();
	bCanCarry = CanCarry();
	bCanGrab = CanGrab();

	if (!bCanClimb && !bCanCarry && !bCanGrab)
	{
		UpdateCanGrabAnim(false);
	}
}

bool AHandController::CanClimb() const
{
	if (bCanClimb) return true;

	UPrimitiveComponent* Component = GetOverlappingComponentWithTag(TEXT("Climbable"));
	return (Component != nullptr);
}

bool AHandController::CanCarry() const
{
	if (bIsCarrying) return true;

	AActor* Actor = GetOverlappingPickupActor();
	if (Actor != nullptr) return true;

	UPrimitiveComponent* Component = GetOverlappingComponentWithTag(TEXT("Carryable"));
	return (Component != nullptr);
}

bool AHandController::CanGrab() const
{
	if (bIsGrabbing) return true;

	UPrimitiveComponent* Component = GetOverlappingComponentWithTag(TEXT("Grabbable"));
	return Component != nullptr;
}

AActor* AHandController::GetOverlappingPickupActor() const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->Implements<UPickupActor>()) return Actor;
	}

	return nullptr;
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

UPrimitiveComponent* AHandController::GetOverlappingComponentWithTag(const FName& Tag) const
{
	TArray<UPrimitiveComponent*> OverlappingComponents;
	GetOverlappingComponents(OverlappingComponents);

	for (UPrimitiveComponent* Component : OverlappingComponents)
	{
		if (Component->ComponentHasTag(Tag)) return Component;
	}

	AActor* Actor = GetOverlappingActorWithTag(Tag);
	if (!Actor) return nullptr;

	UPrimitiveComponent* Component = Actor->FindComponentByClass<UPrimitiveComponent>();
	return Component;
}

UHandAnimInstance* AHandController::GetHandAnimInstance() const
{
	if (!SkeletalMesh) return nullptr;
	return Cast<UHandAnimInstance>(SkeletalMesh->GetAnimInstance());
}

void AHandController::StartFade(float FromAlpha, float ToAlpha, float FadeDuration)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	auto PlayerCameraManager = PlayerController->PlayerCameraManager;
	if (PlayerCameraManager)
	{
		PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, FadeDuration, FLinearColor::Black);
	}
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

void AHandController::TriggerPress()
{
	if (IsHoldingObject())
	{
		UE_LOG(LogTemp, Warning, TEXT("Start action"))
		return;
	}

	ActivateTeleport();
}

void AHandController::TriggerRelease()
{
	if (IsHoldingObject())
	{
		UE_LOG(LogTemp, Warning, TEXT("End action"))
		return;
	}

	if (!bTeleportActive) return;
	BeginTeleport();
}

void AHandController::SetTeleportRight(float AxisValue)
{
	TeleportRightAxis = FMath::Clamp<float>(AxisValue, -1, 1);
}

void AHandController::SetTeleportUp(float AxisValue)
{
	TeleportUpAxis = FMath::Clamp<float>(AxisValue, -1, 1);
}

void AHandController::GripPress()
{
	UpdateGripTypeAnim();
	UpdateGripHeldAnim(true);
	StartClimb();
	StartCarry();
	StartGrab();
}

void AHandController::GripRelease()
{
	UpdateGripHeldAnim(false);
	FinishClimb();
	FinishCarry();
	FinishGrab();
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

void AHandController::UpdateGripTypeAnim()
{
	UHandAnimInstance* HandAnimInstance = GetHandAnimInstance();
	if (!HandAnimInstance) return;

	AActor* Actor = GetOverlappingPickupActor();
	if (Actor && Cast<AGunBase>(Actor))
	{
		HandAnimInstance->SetGripType(EGripType::Pistol);
	}
	else
	{
		HandAnimInstance->SetGripType(EGripType::Default);
	}
}

void AHandController::ActivateTeleport()
{
	if (bTeleportActive) return;

	if (PairedController && PairedController->IsTeleportActive())
	{
		PairedController->ResetTeleport();
	}

	SetTeleportEnabled(true);
	SetTeleportLocked(false);
}

void AHandController::BeginTeleport()
{
	if (!bHasTeleportDestination)
	{
		ResetTeleport();
		return;
	}

	if (bTeleportLocked) return;

	SetTeleportLocked(true);
	StartFade(0.f, 1.f, TeleportFadeDuration);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AHandController::EndTeleport, TeleportFadeDuration);
}

void AHandController::EndTeleport()
{
	FVector Destination = TeleportDestinationMarker->GetComponentLocation();

	ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
	if (Character)
	{
		Destination += Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * GetActorUpVector();
		Character->SetActorLocation(Destination);
		Character->SetActorRotation(TeleportRotation);
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
		PlayerController->SetControlRotation(TeleportRotation);

	StartFade(1.0f, 0.f, TeleportFadeDuration);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AHandController::ResetTeleport, TeleportFadeDuration, false);
}

void AHandController::SetTeleportEnabled(bool bEnabled)
{
	bTeleportActive = bEnabled;
}

void AHandController::SetTeleportLocked(bool bLocked)
{
	bTeleportLocked = bLocked;
}

void AHandController::DrawTeleportArc(const TArray<FVector>& Path)
{
	if (!TeleportPath) return;

	UpdateTeleportPath(Path);

	for (USplineMeshComponent* SplineMesh : TeleportArcMeshPool)
	{
		SplineMesh->SetVisibility(false);
	}

	for (int32 i = 0; i < Path.Num() - 1; i++)
	{
		USplineMeshComponent* SplineMesh = nullptr;
		if (TeleportArcMeshPool.Num() <= i)
		{
			SplineMesh = NewObject<USplineMeshComponent>(this);
			SplineMesh->SetMobility(EComponentMobility::Movable);
			SplineMesh->AttachToComponent(TeleportPath, FAttachmentTransformRules::KeepRelativeTransform);

			if (TeleportArcMesh)
				SplineMesh->SetStaticMesh(TeleportArcMesh);

			if (TeleportArcMaterial)
				SplineMesh->SetMaterial(0, TeleportArcMaterial);

			SplineMesh->RegisterComponent();  // Important for dynamic components

			TeleportArcMeshPool.Add(SplineMesh);
		}

		SplineMesh = TeleportArcMeshPool[i];

		FVector StartLocation, StartTangent, EndLocation, EndTangent;
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent);
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent);

		SplineMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);

		SplineMesh->SetVisibility(true);
	}
}

void AHandController::UpdateTeleportPath(const TArray<FVector>& Path)
{
	if (!TeleportPath) return;

	TeleportPath->ClearSplinePoints(false);
	for (int32 i = 0; i < Path.Num(); i++)
	{
		FVector LocalPosition = TeleportPath->GetComponentTransform().InverseTransformPosition(Path[i]);  // Convert to local coordinates
		FSplinePoint SplinePoint = FSplinePoint(i, LocalPosition, ESplinePointType::Curve);
		TeleportPath->AddPoint(SplinePoint, false);
	}
	TeleportPath->UpdateSpline();
}

void AHandController::UpdateTeleportArrow()
{
	if (!bTeleportActive) return;

	TeleportRotation = FRotator(0.f, GetAttachParentActor()->GetActorRotation().Yaw, 0.f);

	if (FMath::Abs(TeleportUpAxis) > GetThumbDeadZone() || FMath::Abs(TeleportRightAxis) > GetThumbDeadZone())
		TeleportRotation += FVector(TeleportUpAxis, TeleportRightAxis, 0.f).Rotation();

	TeleportArrow->SetWorldRotation(TeleportRotation);
}

void AHandController::UpdateDestinationMarker()
{
	if (!TeleportDestinationMarker) return;

	TArray<FVector> Path;
	FVector Location;

	if (!bTeleportActive)
	{
		ResetTeleport();
		return;
	}

	if (bTeleportLocked)
	{
		TeleportDestinationMarker->SetVisibility(false);
		TeleportArrow->SetVisibility(false);
		TArray<FVector> EmptyPath;
		DrawTeleportArc(EmptyPath);
		return;
	}

	bHasTeleportDestination = FindTeleportDestination(Path, Location);

	if (bHasTeleportDestination)
	{
		TeleportDestinationMarker->SetVisibility(true);
		TeleportDestinationMarker->SetWorldLocation(Location);
		if (GetAttachParentActor())
		{
			TeleportDestinationMarker->SetWorldRotation(GetAttachParentActor()->GetActorRotation());
		}

		TeleportArrow->SetVisibility(true);
	}
	else
	{
		Path.Empty();
		TeleportDestinationMarker->SetVisibility(false);
		TeleportArrow->SetVisibility(false);
	}

	DrawTeleportArc(Path);
	UpdateTeleportArrow();
}

void AHandController::ResetTeleport()
{
	SetTeleportEnabled(false);
	SetTeleportLocked(false);
	TeleportDestinationMarker->SetVisibility(false);
	TeleportArrow->SetVisibility(false);
	TArray<FVector> EmptyPath;
	DrawTeleportArc(EmptyPath);
}

void AHandController::StartClimb()
{
	if (bCanClimb && !bIsClimbing)
	{
		bIsClimbing = true;
		ClimbStartLocation = GetActorLocation();

		if (PairedController && PairedController->bIsClimbing)
			PairedController->GripRelease();

		ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
		if (!Character) return;

		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	}
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

void AHandController::FinishClimb()
{
	if (bIsClimbing)
	{
		bIsClimbing = false;

		ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
		if (!Character) return;

		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	}
}

void AHandController::StartCarry()
{
	if (AttachActor()) return;

	if (!bIsCarrying && bCanCarry)
	{
		UPrimitiveComponent* Component = GetOverlappingComponentWithTag(TEXT("Carryable"));
		if (!Component) return;

		bIsCarrying = true;

		PickUpComponent(Component);

		if (PairedController && PairedController->IsHoldingComponent(Component))
			PairedController->GripRelease();
	}
}

void AHandController::UpdateCarry()
{
	if (!bIsCarrying || !PickupHandle || !PickupLocation) return;

	PickupHandle->SetTargetLocation(PickupLocation->GetComponentLocation());
	PickupHandle->SetTargetRotation(PickupLocation->GetComponentRotation());
}

void AHandController::FinishCarry()
{
	if (ReleaseActor()) return;

	if (bIsCarrying && PickupHandle->GrabbedComponent)
	{
		PickupHandle->ReleaseComponent();
		bIsCarrying = false;
	}
}

void AHandController::StartGrab()
{
	if (!bIsGrabbing && bCanGrab)
	{
		UPrimitiveComponent* Component = GetOverlappingComponentWithTag(TEXT("Grabbable"));
		if (!Component) return;

		bIsGrabbing = true;

		GrabComponent(Component);

		if (PairedController && PairedController->IsHoldingComponent(Component))
			PairedController->GripRelease();
	}
}

void AHandController::UpdateGrab()
{
	if (!bIsGrabbing || !PickupHandle || !GrabSphere) return;

	// Limit location update per frame
	/*
	float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	FVector Location;
	FRotator Rotation;
	PickupHandle->GetTargetLocationAndRotation(Location, Rotation);

	FVector DeltaLocation = Location + (GrabSphere->GetComponentLocation() - Location) * DeltaSeconds;
	PickupHandle->SetTargetLocation(DeltaLocation);
	*/

	PickupHandle->SetTargetLocation(GrabSphere->GetComponentLocation());
}

void AHandController::FinishGrab()
{	
	if (!bIsGrabbing) return;

	if (PickupHandle->GrabbedComponent)
	{
		PickupHandle->ReleaseComponent();
		bIsGrabbing = false;
	}
}

bool AHandController::AttachActor()
{
	HeldActor = GetOverlappingPickupActor();
	if (HeldActor)
	{
		if (PairedController && PairedController->IsHoldingActor(HeldActor))
		{
			PairedController->GripRelease();
		}

		IPickupActor* PickupActor = Cast<IPickupActor>(HeldActor);
		if (!PickupActor) return false;

		PickupActor->Pickup(SkeletalMesh);

		if (MotionController->GetTrackingSource() == EControllerHand::Left)
		{
			PickupActor->InvertRotation();
		}

		return true;
	}
	return false;
}

bool AHandController::ReleaseActor()
{
	if (!HeldActor) return false;

	IPickupActor* PickupActor = Cast<IPickupActor>(HeldActor);
	if (!PickupActor) return false;

	PickupActor->Drop();

	HeldActor = nullptr;
	return true;
}

void AHandController::PickUpComponent(UPrimitiveComponent* Component)
{
	if (!Component || !PickupLocation || !PickupHandle || !GrabSphere) return;

	FVector Location = GrabSphere->GetComponentLocation() - Component->GetComponentLocation();
	FRotator Rotation = GrabSphere->GetComponentRotation() - Component->GetComponentRotation();

	PickupLocation->SetRelativeLocation(Location);
	PickupLocation->SetRelativeRotation(Rotation);

	PickupHandle->GrabComponentAtLocationWithRotation(Component, NAME_None, Component->GetComponentLocation(), Component->GetComponentRotation());
}

void AHandController::GrabComponent(UPrimitiveComponent* Component)
{
	if (!Component || !PickupLocation || !PickupHandle || !GrabSphere) return;

	PickupLocation->SetRelativeLocation(GrabSphere->GetComponentLocation());
	PickupHandle->GrabComponentAtLocation(Component, NAME_None, GrabSphere->GetComponentLocation());
}

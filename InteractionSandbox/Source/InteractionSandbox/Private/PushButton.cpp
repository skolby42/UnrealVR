// Copyright Scott Kolby 2021


#include "PushButton.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

#include "DrawDebugHelpers.h"

APushButton::APushButton()
{
	PrimaryActorTick.bCanEverTick = true;

	Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	SetRootComponent(Base);

	Face = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Face"));
	Face->SetupAttachment(GetRootComponent());
}

void APushButton::BeginPlay()
{
	Super::BeginPlay();
	
	if (!Face) return;
	StartLocation = Face->GetRelativeLocation();
	Face->OnComponentBeginOverlap.AddDynamic(this, &APushButton::OnFaceBeginOverlap);
	Face->OnComponentEndOverlap.AddDynamic(this, &APushButton::OnFaceEndOverlap);

	StartOffset = FVector::ZeroVector;
}

void APushButton::OnFaceBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("BeginOverlap"));

	//StartOffset = GetOverlapVerticalOffset();

	/*if (!Face) return;
	Face->AddRelativeLocation(FVector(0.f, 0.f, -1.f));*/
}

void APushButton::OnFaceEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("EndOverlap"))

	StartOffset = FVector::ZeroVector;
}

void APushButton::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//GetOverlapVerticalOffset();

	FHitResult HitResult;
	FVector Start = Face->GetComponentLocation();
	FVector End = Start + Face->GetUpVector() * 10.f;  // TODO remove magic number
	FCollisionQueryParams CollisionParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionParams.AddIgnoredActor(this);

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.f, (uint8)'\000', 2.f);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);
	if (bHit)
	{
		FVector HitLocation = HitResult.Location;
		FVector NewFaceLocation = Face->GetComponentLocation();
		NewFaceLocation.Z = HitLocation.Z;
		Face->SetWorldLocation(NewFaceLocation);
	}
	//FVector CurrentOffset = StartOffset - GetOverlapVerticalOffset();

	//if (CurrentOffset != FVector::ZeroVector)
	//{
	//	FVector NewLocation = Face->GetComponentLocation() + CurrentOffset;
	//	Face->SetWorldLocation(NewLocation);

	//	//UE_LOG(LogTemp, Warning, TEXT("NewLocation: %s"), *NewLocation.ToString())
	//}
}

FVector APushButton::GetOverlapVerticalOffset()
{
	FVector Offset = FVector::ZeroVector;
	if (!Face) return Offset;

	TSet<UPrimitiveComponent*> OverlappingComponents;
	GetOverlappingComponents(OverlappingComponents);

	for (UPrimitiveComponent* Component : OverlappingComponents)
	{
		Offset = FVector(0.f, 0.f, Component->GetComponentLocation().Z - Face->GetComponentLocation().Z);
		break;
	}

	return Offset;
}

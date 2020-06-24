// Copyright Scott Kolby 2020


#include "Projectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GunBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(GetRootComponent());

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(GetRootComponent());

	ImpactBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("ImpactBlast"));
	ImpactBlast->SetupAttachment(GetRootComponent());
	ImpactBlast->SetAutoActivate(false);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetAutoActivate(false);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Cast<AGunBase>(OtherActor)) return;
	UE_LOG(LogTemp, Warning, TEXT("OnHit"))
}

void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AGunBase>(OtherActor)) return;

	ImpactBlast->Activate();

	if (OtherComp->IsSimulatingPhysics())
	{
		FVector Impulse = ProjectileMovement->Velocity.Size() * GetActorForwardVector();
		OtherComp->AddImpulseAtLocation(Impulse, CollisionComponent->GetComponentLocation());
	}

	ProjectileMesh->DestroyComponent();

	UGameplayStatics::ApplyDamage(
		OtherActor,
		ProjectileDamage,
		GetWorld()->GetFirstPlayerController(),
		this,
		UDamageType::StaticClass());

	// Set destroy timer
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AProjectile::OnTimerExpired, DestroyDelay);
}

void AProjectile::OnTimerExpired()
{
	Destroy();
}

void AProjectile::Launch(float Speed)
{
	if (!ProjectileMovement) return;

	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
	ProjectileMovement->Activate();
}


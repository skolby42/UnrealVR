// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class INTERACTIONSANDBOX_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	void Launch(float Speed);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnTimerExpired();
	
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* ImpactBlast = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float ProjectileDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float DestroyDelay = 1.f;
};

// Copyright Scott Kolby 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

UCLASS()
class LIGHTPAINTER_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	AVRPawn();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:

	void CreateHandControllers();

	void Save();
	void Load();

	// Pass through methods
	void TriggerPressed();
	void TriggerReleased();

private:

	// Components
	UPROPERTY(VisibleAnywhere)
	USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera = nullptr;

	// References
	UPROPERTY()
	class AHandControllerBase* LeftController = nullptr;
	
	UPROPERTY()
	class AHandControllerBase* RightController = nullptr;

	// Config
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHandControllerBase> HandControllerClass;
};

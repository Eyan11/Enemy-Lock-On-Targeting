/*
* Author: Eyan Martucci
* Description: Manages player input, movement, and camera rotation
*/

#pragma once

#include "EnhancedInputComponent.h"			// Input
#include "EnhancedInputSubsystems.h"		// Input

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"


UCLASS()
class ENEMYLOCKONTARGETING_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


//*********************************************************

protected:

	// *** Components
	UPROPERTY(EditAnywhere, Category = "Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Components")
	class UCameraComponent* Camera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class ULockOnTargeting* LockOnTargetingComp;

	// *** Input
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* AttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LockOnTargetAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SwitchToLeftTargetAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SwitchToRightTargetAction;

	bool bIsHoldingLockOnTargetingInput = false;

	// *** Movement Settings
	FVector InputDir = FVector::ForwardVector;
	float StartMovingCounter = 0.0f;
	bool bIsDelayingMovement = false;
	bool bIsSwitchingDir = false;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
	float RotationLerp = 12.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
	float InputDeadzone = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
	float StartMovingDelay = 0.2f;

	// *** Camera Settings
	UPROPERTY(EditDefaultsOnly, Category = "Camera Settings")
	float CameraSensitivity = 0.7f;

	// *** Functions
	
	// Input
	void Move(const FInputActionValue& Value);
	void StartJump();
	void Look(const FInputActionValue& Value);
	void StartLockOnTargeting();
	void StopLockOnTargeting();
	void SwitchToLeftTarget();
	void SwitchToRightTarget();

	// Helper
	void CalculateMoveStateChanges(const FVector& PlayerForward);
	void UpdatePlayerRotation();

};

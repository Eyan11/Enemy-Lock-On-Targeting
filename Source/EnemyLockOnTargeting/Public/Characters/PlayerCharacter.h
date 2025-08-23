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

public:

	void StopMoveInput();		// Stops player from moving and rotating
	void ResumeMoveInput();		// Allows player to move and rotate

	bool IsTargetingInputHeld() const { return bIsHoldingTargetingInput; }
	class UPlayerMeleeCombat* GetMeleeCombatComponent() { return MeleeCombatComp; }
	class UCapsuleComponent* GetSwordCollision() {return SwordCollision; }


private:

	// *** Components
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USkeletalMeshComponent* SwordSkeletalMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UCapsuleComponent* SwordCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UStaticMeshComponent* ShieldStaticMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UBoxComponent* ShieldCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class ULockOnTargeting* LockOnTargetingComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UPlayerMeleeCombat* MeleeCombatComp;

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
	class UInputAction* LockOnTargetAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SwitchToLeftTargetAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SwitchToRightTargetAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* AttackAction;

	bool bIsHoldingTargetingInput = false;

	// *** Settings
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float InputDeadzone = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float CameraSensitivity = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")		// Player rotation speed towards movement direction
	float RotationLerp = 12.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float StartMovingDelay = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")		// The percent of the normal speed that the player moves at when targeting
	float TargetingSpeedMultiplier = 0.5f;

	class UAIPerceptionStimuliSourceComponent* StimulusSource;	// Player is a stimulus source, meaning it can be detected by enemy AI perception
	FVector InputDir = FVector::ForwardVector;				// The last input direction
	float StartMovingCounter = 0.0f;								
	bool bIsDelayingMovement = false;
	bool bIsSwitchingDir = false;
	bool bIsMoveInputAllowed = true;

	// *** Functions
	
	// Input
	void Move(const FInputActionValue& Value);
	void StartJump();
	void Look(const FInputActionValue& Value);
	void StartLockOnTargeting();
	void StopLockOnTargeting();
	void SwitchToLeftTarget();
	void SwitchToRightTarget();
	void Attack();

	// Helper
	void CalculateMoveStateChanges(FVector playerForward);
	void UpdatePlayerRotation();

};

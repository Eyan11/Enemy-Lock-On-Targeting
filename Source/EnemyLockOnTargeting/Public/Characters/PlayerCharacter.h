/*
* Author: Eyan Martucci
* Description: Manages player input and basic movement
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
	UPROPERTY(EditAnywhere, Category = "Components")	// Visible and editable 
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Components")	// Visible and editable
	class UCameraComponent* Camera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")	// Visible in editor; Blueprints can only read, no write perms 
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

	void Move(const FInputActionValue& Value);
	void StartJump();
	void Look(const FInputActionValue& Value);
	void StartLockOnTargeting();
	void StopLockOnTargeting();

	void CalculateMoveStateChanges(const FVector& PlayerForward);


};

/*
* Author: Eyan Martucci
* Description: Manages player input, movement, and camera rotation
*/

#include "Characters/PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"			// Spring Arm
#include "Camera/CameraComponent.h"						// Camera
#include "GameFramework/CharacterMovementComponent.h"	// Character Movement
#include "Components/LockOnTargeting.h"					// Lock on Targeting
#include "Components/PlayerMeleeCombat.h"				// Melee Combat
#include "Components/SkeletalMeshComponent.h"			// Skeletal Mesh (to hold sword)
#include "Components/CapsuleComponent.h"				// Capsule Collision
#include "Components/BoxComponent.h"					// Box Collision


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Spring Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;		// Let spring arm handle look rotation

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// Sword and Shield Mesh
	SwordSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sword Skeletal Mesh"));
	SwordSkeletalMesh->SetupAttachment(GetMesh(), TEXT("RightHandWeapon"));	
	SwordSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield Static Mesh"));
	ShieldStaticMesh->SetupAttachment(GetMesh(), TEXT("LeftHandShield"));
	ShieldStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Sword and Shield Collision
	SwordCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sword Collision MAIN"));
	SwordCollision->SetupAttachment(SwordSkeletalMesh);
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Shield Collision"));
	ShieldCollision->SetupAttachment(ShieldStaticMesh);
	ShieldCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Custom Components
	LockOnTargetingComp = CreateDefaultSubobject<ULockOnTargeting>(TEXT("Lock On Targeting Component"));
	MeleeCombatComp = CreateDefaultSubobject<UPlayerMeleeCombat>(TEXT("Melee Combat Component"));

	// Class Defaults
	bUseControllerRotationYaw = false;

	// Initialize Variables
	StartMovingCounter = StartMovingDelay;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC) return;

	UEnhancedInputLocalPlayerSubsystem* subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!subsystem) return;

	subsystem->AddMappingContext(InputMappingContext, 0);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bCanMove)
		UpdatePlayerRotation();
}

// Locks player rotation every frame depending on lock on targeting mode
void APlayerCharacter::UpdatePlayerRotation() {

	// *** Lock Rotation on Actor Target
	if (bIsHoldingTargetingInput && LockOnTargetingComp->bIsTargeting) {

		if (!LockOnTargetingComp->TargetedActor) return;	// Stop if actor is destroyed

		FRotator targetRotation =
			(LockOnTargetingComp->TargetedActor->GetActorLocation() - GetActorLocation()).Rotation();
		targetRotation.Pitch = 0.0f;
		targetRotation.Roll = 0.0f;

		FRotator newRotation = FMath::RInterpTo(GetActorRotation(),
			targetRotation, GetWorld()->GetDeltaSeconds(), RotationLerp);
		SetActorRotation(newRotation);
	}
	// *** Rotate to Move Direction
	else if (!bIsHoldingTargetingInput) {

		FRotator TargetRotation = InputDir.Rotation();
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(),
			TargetRotation, GetWorld()->GetDeltaSeconds(), RotationLerp);
		SetActorRotation(NewRotation);
	}
	// If holding targeting input but not targeting an actor, don't rotate player
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to Enhanced Input Component
	UEnhancedInputComponent* enhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!enhancedInput) return;

	// Bind the input action event to a method
	enhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	enhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::StartJump);
	enhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	enhancedInput->BindAction(LockOnTargetAction, ETriggerEvent::Started, this, &APlayerCharacter::StartLockOnTargeting);
	enhancedInput->BindAction(LockOnTargetAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopLockOnTargeting);
	enhancedInput->BindAction(SwitchToLeftTargetAction, ETriggerEvent::Started, this, &APlayerCharacter::SwitchToLeftTarget);
	enhancedInput->BindAction(SwitchToRightTargetAction, ETriggerEvent::Started, this, &APlayerCharacter::SwitchToRightTarget);
	enhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::Attack);
}


// Moves player relative to camera
void APlayerCharacter::Move(const FInputActionValue& Value) {
	
	if (!bCanMove) return;

	FVector2D moveInput = Value.Get<FVector2D>();
	moveInput = moveInput.GetSafeNormal();		// Prevent faster diagonal movement

	// *** Check For Special Cases
	if (GetCharacterMovement()->IsFalling() || moveInput.Size() < InputDeadzone) {
		bIsDelayingMovement = false;
		return;									// No movement if falling or small input
	}


	// *** Calculate Input Direction Relative to Camera
	FVector camForward = Camera->GetForwardVector();
	camForward.Z = 0.0f;
	camForward.Normalize();

	FVector camRight = Camera->GetRightVector();
	camRight.Z = 0.0f;
	camRight.Normalize();

	InputDir = (camForward * moveInput.Y) + (camRight * moveInput.X);
	InputDir.Normalize();						// Prevent faster diagonal movement

	FVector playerForward = GetActorForwardVector();


	// *** Apply Targeting Movement
	if (bIsHoldingTargetingInput) {

		// *** Move at Normal Speed When Moving Forward
		if (FVector::DotProduct(InputDir, playerForward) > 0.71)
			AddMovementInput(InputDir, moveInput.Size());

		// *** Otherwise Move at Reduced Speed
		else
			AddMovementInput(InputDir, TargetingSpeedMultiplier * moveInput.Size());
	}

	// *** Apply Normal Movement
	else {
		// *** Check for Move Delay or Switch Directions
		CalculateMoveStateChanges(playerForward);

		if(bIsSwitchingDir)
			AddMovementInput(InputDir, moveInput.Size());		// Switch directions movement
		else if(!bIsDelayingMovement)
			AddMovementInput(playerForward, moveInput.Size());	// Normal movement
	}
}

// Helper method for Move() which checks if player just started moving from idle and if switching directions
void APlayerCharacter::CalculateMoveStateChanges(FVector playerForward) {

	// *** Check to Start Moving Delay
	if (!bIsDelayingMovement && GetVelocity().IsNearlyZero()) {		// If idle and not already delaying movement

		if (FVector::DotProduct(InputDir, playerForward) < 0.80) {	// If not facing input direction, delay movement
			bIsDelayingMovement = true;
			StartMovingCounter = StartMovingDelay;
			return;													// Cannot switch directions while delaying movement
		}
	}

	// *** Check to End Moving Delay
	if (bIsDelayingMovement) {
		StartMovingCounter -= GetWorld()->GetDeltaSeconds();

		if (StartMovingCounter <= 0)			// If move delay counter expired, allow movement
			bIsDelayingMovement = false;
		return;									// Cannot switch directions while delaying movement
	}


	float dotProd = FVector::DotProduct(InputDir, playerForward);

	// *** Check to Start Switching Directions
	if (!bIsSwitchingDir && dotProd < -0.80)	// If input is in opposite direction
		bIsSwitchingDir = true;

	// *** Check to Stop Switching Directions
	if (bIsSwitchingDir && dotProd > 0.99)		// If input is in forward direction
		bIsSwitchingDir = false;
}

// Applies jump force when player presses jump input
void APlayerCharacter::StartJump() {
	Jump();
}

// Rotates player camera left, right, up, and down 
void APlayerCharacter::Look(const FInputActionValue& Value) {

	FVector2D lookInput = Value.Get<FVector2D>();

	// *** Apply Look
	if (!LockOnTargetingComp->bIsTargeting) {	// Let component handle look when targeting
		AddControllerYawInput(CameraSensitivity * lookInput.X);
		AddControllerPitchInput(CameraSensitivity * -lookInput.Y);
	}

	// *** Give Lock On Targeting Component Look Input
	if(bIsHoldingTargetingInput && !lookInput.IsNearlyZero())
		LockOnTargetingComp->OnLookInput(lookInput);
}

// Starts lock on targeting
void APlayerCharacter::StartLockOnTargeting() {
	bIsHoldingTargetingInput = true;

	LockOnTargetingComp->OnTargetingInputStart();
}

// Ends lock on targeting
void APlayerCharacter::StopLockOnTargeting() {
	bIsHoldingTargetingInput = false;
	InputDir = GetActorForwardVector();			// When returning to normal movement, face forward

	LockOnTargetingComp->OnTargetingInputEnd();
}

// Switches current target to the closest target to the left of current one
void APlayerCharacter::SwitchToLeftTarget() {
	LockOnTargetingComp->OnSwitchDirectionalTargetInput(false);
}

// Switches current target to the closest target to the right of current one
void APlayerCharacter::SwitchToRightTarget() {
	LockOnTargetingComp->OnSwitchDirectionalTargetInput(true);
}

// Starts attack animation on hitbox detection
void APlayerCharacter::Attack() {
	MeleeCombatComp->OnAttackInput();
}

// Stops player from moving and rotating
void APlayerCharacter::StopMovement() {
	bCanMove = false;
	InputDir = GetActorForwardVector();		// Reset to default
}

// Allows player to move and rotate
void APlayerCharacter::ResumeMovement() {
	bCanMove = true;
}

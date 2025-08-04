/*
* Author: Eyan Martucci
* Description: Manages player input, movement, and camera rotation
*/

#include "Characters/PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"			// Spring Arm
#include "Camera/CameraComponent.h"						// Camera
#include "GameFramework/CharacterMovementComponent.h"	// Character Movement Component
#include "Components/LockOnTargeting.h"					// Lock on targeting

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

	// Lock on targeting
	LockOnTargetingComp = CreateDefaultSubobject<ULockOnTargeting>(TEXT("LockOnTargetingComponent"));

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

	UpdatePlayerRotation();
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
}

// Helper method for Move() which checks if player just started moving from idle and if switching directions
void APlayerCharacter::CalculateMoveStateChanges(const FVector& playerForward) {

	// *** Start Moving Delay

	// If player just started moving from idle
	if (!bIsDelayingMovement && GetVelocity().IsNearlyZero()) {

		// If player is NOT facing input direction, delay movement
		if (FVector::DotProduct(InputDir, playerForward) < 0.80) {
			bIsDelayingMovement = true;
			StartMovingCounter = StartMovingDelay;
		}
	}

	// If player is waiting for delay to end before moving from idle
	if (bIsDelayingMovement) {
		StartMovingCounter -= GetWorld()->GetDeltaSeconds();

		if (StartMovingCounter <= 0)
			bIsDelayingMovement = false;
	}


	// *** Switch Directions

	// If player wants to move in opposite direction
	if (!bIsSwitchingDir && FVector::DotProduct(InputDir, playerForward) < -0.80)
		bIsSwitchingDir = true;

	// If player has finished switching directions
	if (bIsSwitchingDir && FVector::DotProduct(InputDir, playerForward) > 0.97)
		bIsSwitchingDir = false;
}


// Moves player relative to camera
void APlayerCharacter::Move(const FInputActionValue& Value) {
	
	FVector2D moveInput = Value.Get<FVector2D>();

	// *** Check For Special Cases
	if (GetCharacterMovement()->IsFalling() || moveInput.Size() < InputDeadzone) {
		bIsDelayingMovement = false;
		return;									// No movement if falling or small input
	}

	FVector playerForward = GetActorForwardVector();
	CalculateMoveStateChanges(playerForward);	// Check to delay movement or switch directions


	// *** Calculate Input Direction Relative to Camera
	FVector camForward = Camera->GetForwardVector();
	camForward.Z = 0.0f;
	camForward.Normalize();

	FVector camRight = Camera->GetRightVector();
	camRight.Z = 0.0f;
	camRight.Normalize();

	InputDir = (camForward * moveInput.Y) + (camRight * moveInput.X);
	InputDir.Normalize();

	if (bIsDelayingMovement) return;				// Don't apply movement if delaying movement

	// *** Apply Movement
	if (bIsSwitchingDir) {
		AddMovementInput(InputDir, moveInput.Size());		// Switch directions movement
	}
	else if (bIsHoldingLockOnTargetingInput) {
		AddMovementInput(camForward, moveInput.Y);			// Targeting movement
		AddMovementInput(camRight, moveInput.X);
	}
	else {
		AddMovementInput(playerForward, moveInput.Size());	// Normal movement
	}
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
	if(bIsHoldingLockOnTargetingInput && !lookInput.IsNearlyZero())
		LockOnTargetingComp->OnLookInput(lookInput);
}

// Starts lock on targeting
void APlayerCharacter::StartLockOnTargeting() {
	bIsHoldingLockOnTargetingInput = true;

	LockOnTargetingComp->OnTargetingInputStart();
}

// Ends lock on targeting
void APlayerCharacter::StopLockOnTargeting() {
	bIsHoldingLockOnTargetingInput = false;
	InputDir = GetActorForwardVector();			// When returning to normal movement, face forward

	LockOnTargetingComp->OnTargetingInputEnd();
}

// Locks player rotation every frame depending on lock on targeting mode
void APlayerCharacter::UpdatePlayerRotation() {

	// *** Lock Rotation on Actor Target
	if (bIsHoldingLockOnTargetingInput && LockOnTargetingComp->bIsTargeting) {

		if (!LockOnTargetingComp->TargetedActor) return;	// Stop if actor is destroyed

		FRotator TargetRotation =
			(LockOnTargetingComp->TargetedActor->GetActorLocation() - GetActorLocation()).Rotation();
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(),
			TargetRotation, GetWorld()->GetDeltaSeconds(), RotationLerp);
		SetActorRotation(NewRotation);
	}
	// *** Rotate to Move Direction
	else if(!bIsHoldingLockOnTargetingInput) {

		FRotator TargetRotation = InputDir.Rotation();
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(),
			TargetRotation, GetWorld()->GetDeltaSeconds(), RotationLerp);
		SetActorRotation(NewRotation);
	}
	// If holding targeting input but not targeting an actor, don't rotate player
}

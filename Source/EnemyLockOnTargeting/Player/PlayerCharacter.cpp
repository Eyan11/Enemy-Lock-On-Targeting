// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyLockOnTargeting/Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"						// Camera
#include "GameFramework/SpringArmComponent.h"			// Spring Arm
#include "GameFramework/CharacterMovementComponent.h"	// Character Movement Component

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

	// Class Defaults
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	StartMovingCounter = StartMovingDelay;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	APlayerController* PC = Cast<APlayerController>(Controller);

	if (PC) {
		UEnhancedInputLocalPlayerSubsystem* subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		
		if (subsystem)
			subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// *** Player Rotation
	FRotator TargetRotation = InputDir.Rotation();
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(),
		TargetRotation, GetWorld()->GetDeltaSeconds(), RotationLerp);
	SetActorRotation(NewRotation);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to Enhanced Input Component
	UEnhancedInputComponent* enhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// Bind the input action event to a method
	if (enhancedInput) {
		enhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		enhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::StartJump);
		enhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	}
}


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


/* Input: Moves player forwards, backwards, left, and right */
void APlayerCharacter::Move(const FInputActionValue& Value) {
	
	FVector2D moveInput = Value.Get<FVector2D>();

	// If grounded or no input or input is too small, exit function
	if (GetCharacterMovement()->IsFalling() || moveInput.Size() < InputDeadzone) {
		bIsDelayingMovement = false;
		return;
	}

	FVector playerForward = GetActorForwardVector();

	// Get camera XY vectors
	FVector camForward = Camera->GetForwardVector();
	camForward.Z = 0.0f;
	camForward.Normalize();

	FVector camRight = Camera->GetRightVector();
	camRight.Z = 0.0f;
	camRight.Normalize();

	// Get input direction in reference to camera
	InputDir = (camForward * moveInput.Y) + (camRight * moveInput.X);
	InputDir.Normalize();

	// Checks for start moving delay and switching directions
	CalculateMoveStateChanges(playerForward);

	// Apply movement
	if (!bIsDelayingMovement) {

		// If switching directions, move in input direction
		if(bIsSwitchingDir)
			AddMovementInput(InputDir, moveInput.Size());
		// Otherwise move in player forward direction
		else
			AddMovementInput(playerForward, moveInput.Size());
	}
}

/* Input: Applies jump force when player presses jump input */
void APlayerCharacter::StartJump() {

	Jump();
}

/* Input: Rotates player camera left, right, up, and down */
void APlayerCharacter::Look(const FInputActionValue& Value) {

	FVector2D lookVector = Value.Get<FVector2D>();

	AddControllerYawInput(lookVector.X);	// Look left/right
	AddControllerPitchInput(-lookVector.Y);	// Look up/down (inverted)
}


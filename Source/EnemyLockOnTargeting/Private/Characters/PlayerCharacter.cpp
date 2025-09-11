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
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Characters/EnemyCharacter.h"					// Enemy Character (to spawn)


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
	SwordCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sword Collision"));
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

	// Stimulus Source
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if (!StimulusSource) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("StimulusSource is null in PlayerController"));
		return;
	}
	StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimulusSource->RegisterWithPerceptionSystem();
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

	if(bIsMoveInputAllowed)
		UpdatePlayerRotation();

	if (JumpTimer > 0)
		JumpTimer -= DeltaTime;
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
	enhancedInput->BindAction(SpawnEnemyAction, ETriggerEvent::Started, this, &APlayerCharacter::SpawnEnemy);
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	JumpTimer = JumpDelay;
}


// Moves player relative to camera
void APlayerCharacter::Move(const FInputActionValue& Value) {
	
	if (!bIsMoveInputAllowed) return;

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
		if (FVector::DotProduct(InputDir, playerForward) > 0.9)
			AddMovementInput(InputDir, TargetingForwardWalkSpeedMultiplier * moveInput.Size());

		// *** Otherwise Move at Reduced Speed
		else
			AddMovementInput(InputDir, TargetingWalkSpeedMultiplier * moveInput.Size());
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

	if (GetCharacterMovement()->IsFalling() || JumpTimer > 0 || !bIsMoveInputAllowed)
		return;

	// *** Directional Jump While Targeting
	if (bIsHoldingTargetingInput) {
		FVector curVel = GetVelocity().GetSafeNormal2D();
		GetCharacterMovement()->StopMovementImmediately();
		Jump();

		if (curVel.IsNearlyZero())				// If not moving, only jump upwards
			return;

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::Printf(TEXT("curVel: %s"), *curVel.ToString()));

		// Get angles between velocity and player forward/right vectors
		float forwardDotProd = FVector::DotProduct(curVel, GetActorForwardVector().GetSafeNormal2D());
		float rightDotProd = FVector::DotProduct(curVel, GetActorRightVector().GetSafeNormal2D());

		// *** Apply Directional Jump
		if (forwardDotProd > 0.7)			// Jump forward
			GetCharacterMovement()->AddImpulse(GetActorForwardVector() * JumpDirectionalImpulse);
		else if (forwardDotProd < -0.7)		// Jump backwards
			GetCharacterMovement()->AddImpulse(-GetActorForwardVector() * JumpDirectionalImpulse);
		else if (rightDotProd > 0)			// Jump right
			GetCharacterMovement()->AddImpulse(GetActorRightVector() * JumpDirectionalImpulse);
		else								// Jump left
			GetCharacterMovement()->AddImpulse(-GetActorRightVector() * JumpDirectionalImpulse);
	}
	// *** Jump in Move Direction
	else {
		Jump();
	}
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

// Starts attack montage if player is grounded
void APlayerCharacter::Attack() {
	if(!GetCharacterMovement()->IsFalling())
		MeleeCombatComp->OnAttackInput();
}

// Stops player from moving and rotating
void APlayerCharacter::StopMoveInput() {
	bIsMoveInputAllowed = false;
	InputDir = GetActorForwardVector();		// Reset to default
}

// Allows player to move and rotate
void APlayerCharacter::ResumeMoveInput() {
	bIsMoveInputAllowed = true;
}

// Spawns an enemy in the direction of player camera
void APlayerCharacter::SpawnEnemy() {

	if (!EnemyToSpawn && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EnemyToSpawn is null in PlayerCharacter->SpawnEnemy"));
		return;
	}

	// *** Spawn Enemy
	FVector spawnLoc = GetActorLocation() +		// Spawn in camera forward direction in the air
		(Camera->GetForwardVector().GetSafeNormal2D() * EnemySpawnDistance) + (GetActorUpVector() * 100.0f);
	FActorSpawnParameters SpawnParams;
	AActor* newEnemy = GetWorld()->SpawnActor<AActor>(EnemyToSpawn,	spawnLoc, GetActorRotation(), SpawnParams);

	// Check if Spawn Worked
	if (!newEnemy && GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Enemy failed to spawn in PlayerCharacter->SpawnEnemy"));
}
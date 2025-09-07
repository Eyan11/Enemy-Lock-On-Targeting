/*
* Author: Eyan Martucci
* Description: Oversees the entire enemy class, manages movement and attack
*/

#include "Characters/EnemyCharacter.h"
#include "Components/EnemyHealth.h"						// Enemy Health Component
#include "Controllers/EnemyAIController.h"				// Enemy AI Controller
#include "NavigationSystem.h"							// Nav Mesh
#include "GameFramework/CharacterMovementComponent.h"	// Character Movement
#include "Components/CapsuleComponent.h"				// Capsule Collision
#include "Animation/EnemyAnimInstance.h"				// Enemy Anim Instance
#include "DrawDebugHelpers.h"							// Draw Debug Capsule
#include "Kismet/GameplayStatics.h"						// Apply Damage
#include "Characters/PlayerCharacter.h"					// Player Character
#include "Components/WidgetComponent.h"					// Widget Component

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Components
	HealthComponent = CreateDefaultSubobject<UEnemyHealth>(TEXT("Health Component"));

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthValue"));
	WidgetComp->SetupAttachment(RootComponent);
	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	// Sword mesh and collision
	SwordStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword Static Mesh"));
	SwordStaticMesh->SetupAttachment(GetMesh(), TEXT("RightHandSword"));
	SwordStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sword Collision"));
	SwordCollision->SetupAttachment(SwordStaticMesh);
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// AI Controller
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Class Defaults
	GetCharacterMovement()->MaxWalkSpeed = RoamingSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;	// Let movement decide rotation
	bUseControllerRotationYaw = false;							// Don't use controller rotation
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// *** Check Montage Reference
	if (!AttackMontage && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AttackMontage is null in EnemyCharacter"));
	}

	// *** Get Enemy Anim Instance Reference
	EnemyAnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	if (!EnemyAnimInstance && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("EnemyAnimInstance is null in EnemyCharacter"));
		return;
	}

	// *** Get Enemy AI Controller Reference
	EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (!EnemyAIController && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EnemyAIController Cast failed in EnemyCharacter"));
		return;
	}

	// Check Sword Collision, healthbar widget, and health component
	if ((!SwordCollision || !HealthComponent) && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("SwordCollision or HealthComponent is null in EnemyCharacter"));
		return;
	}

	// Check healthbar widget class and component
	if ((!HealthbarWidgetClass || !WidgetComp) && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("HealthbarWidgetClass or WidgetComp is null in EnemyCharacter"));
		return;
	}

	// Specify that the widget is a UEnemyHealthbarWidget
	WidgetComp->SetWidgetClass(HealthbarWidgetClass);
	// Store reference to UEnemyHealthBarWidget to reference its properties later
	HealthbarWidget = Cast<UEnemyHealthbarWidget>(WidgetComp->GetUserWidgetObject());

	if (!HealthbarWidget && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("HealthbarWidget is null in EnemyCharacter"));
		return;
	}

	// *** Bind Sword Collision Overlap Method
	SwordCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnSwordBeginOverlap);
	EnemyAnimInstance->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnMontageEnd);
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


// Changes move speed depending on enemy state
void AEnemyCharacter::SwitchMoveState(EEnemyMoveState newState) {

	CurState = newState;

	switch (CurState) {

		case EEnemyMoveState::Roaming:
			GetCharacterMovement()->MaxWalkSpeed = RoamingSpeed;
			GetCharacterMovement()->bOrientRotationToMovement = true;	// Let movement decide rotation
			bUseControllerRotationYaw = false;							// Don't use controller rotation
			break;

		case EEnemyMoveState::Chasing:
			GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			bUseControllerRotationYaw = true;
			break;

		case EEnemyMoveState::Retreating:
			GetCharacterMovement()->MaxWalkSpeed = RetreatingSpeed;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			bUseControllerRotationYaw = true;
			break;
	}
}


// Plays attack montage
void AEnemyCharacter::StartAttacking() {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("ATTACK!"));

	if (!EnemyAnimInstance && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EnemyAnimInstance is null in EnemyCharacter"));
		return;
	}

	// *** Play Attack Montage
	EnemyAnimInstance->Montage_Play(AttackMontage);
}


// Applies damage to sword collidee
void AEnemyCharacter::OnSwordBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	if (OtherActor && GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("Enemy Sword Collision OVERLAPING: ") + OtherActor->GetName());
	else if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("OtherActor is null in EnemyCharacter->OnEnemySwordBeginOverlap()"));
	
	// *** Apply Damage to Player
	if (OtherActor && Cast<APlayerCharacter>(OtherActor)) {

		UGameplayStatics::ApplyDamage(
			OtherActor,						// Actor to damage
			SwordDamage,					// Damage amount
			GetInstigatorController(),		// Event instigator
			this,							// Damage cause
			UDamageType::StaticClass()		// Damage type class
		);
	}
}


// Enables sword collision during the attacking animation
void AEnemyCharacter::EnableAttackCollision() {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("Enemy Sword Collision ENABLED"));

	// *** Enable Sword Overlap Collision
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


// Disables sword collision during the attacking animation
void AEnemyCharacter::DisableAttackCollision() {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("Enemy Sword Collision DISABLED"));

	// *** Disable Sword Collision
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


// Starts transition to the retreat state
void AEnemyCharacter::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
		EnemyAIController->OnFinishAttack();
}


void AEnemyCharacter::StopMovementOnDeath() {

	GetCharacterMovement()->DisableMovement();

	if (!EnemyAIController && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EnemyAIController is null in EnemyCharacter"));
		return;
	}

	EnemyAIController->StopMovement();
	DetachFromControllerPendingDestroy();
}
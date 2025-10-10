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

	// Get References
	EnemyAnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	EnemyAIController = Cast<AEnemyAIController>(GetController());

	// Specify that the widget is a UEnemyHealthbarWidget
	WidgetComp->SetWidgetClass(HealthbarWidgetClass);
	// Store reference to UEnemyHealthBarWidget to access its properties later
	HealthbarWidget = Cast<UEnemyHealthbarWidget>(WidgetComp->GetUserWidgetObject());

	// Bind functions
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
void AEnemyCharacter::StartAttacking() 
{
	EnemyAnimInstance->Montage_Play(AttackMontage);		// Play attack montage
}


// Applies damage to sword collidee
void AEnemyCharacter::OnSwordBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	// Apply damage to player
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
void AEnemyCharacter::EnableAttackCollision() 
{
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);		// Enable sword collision
}


// Disables sword collision during the attacking animation
void AEnemyCharacter::DisableAttackCollision() 
{
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// Disable sword collision
}


// Called when a montage ends, handles transition from attack to retreat
void AEnemyCharacter::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
		EnemyAIController->OnFinishAttack();	// Starts transition to the retreat state
}


// Called when enemy dies, disables movement and enemy AI
void AEnemyCharacter::StopMovementOnDeath() {

	GetCharacterMovement()->DisableMovement();

	EnemyAIController->StopMovement();
	DetachFromControllerPendingDestroy();
}
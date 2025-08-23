/*
* Author: Eyan Martucci
* Description:
*/

#include "Characters/EnemyCharacter.h"
#include "Components/EnemyHealth.h"			// Enemy Health Component
#include "Controllers/EnemyAIController.h"	// Enemy AI Controller
#include "NavigationSystem.h"				// Nav Mesh

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	HealthComponent = CreateDefaultSubobject<UEnemyHealth>(TEXT("Health Component"));

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (!EnemyAIController) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EnemyAIController Cast failed in EnemyCharacter"));
		return;
	}

	EnemyAIController->MoveToRandomLocation();		// Start moving
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


// Plays attack montage
void AEnemyCharacter::StartAttacking() {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("ATTACK!"));

	// TODO - call after attack animation finishes
	EnemyAIController->SwitchEnemyState(EEnemyState::Retreating);
}

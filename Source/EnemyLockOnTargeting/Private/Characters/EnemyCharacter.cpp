/*
* Author: Eyan Martucci
* Description:
*/

#include "Characters/EnemyCharacter.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

// In your character/actor source (.cpp)
float AEnemyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
    class AController* EventInstigator, AActor* DamageCauser)
{
    float damageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("Enemy Took Damage"));

    return damageAmount;	// Return the actual damage applied
}



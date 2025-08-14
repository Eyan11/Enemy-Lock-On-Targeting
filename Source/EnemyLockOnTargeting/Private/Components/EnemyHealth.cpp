/*
* Author: Eyan Martucci
* Description:
*/

#include "Components/EnemyHealth.h"

#include "Characters/EnemyCharacter.h"	// Enemy Character

// Sets default values for this component's properties
UEnemyHealth::UEnemyHealth()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyHealth::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner()) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GetOwner() is null in EnemyHealth"));
		return;
	}

	// *** Get Enemy Character Reference
	EnemyCharacter = Cast<AEnemyCharacter>(GetOwner());
	if (!EnemyCharacter) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EnemyCharacter is null in EnemyHealth"));
		return;
	}

	// *** Subscribe Function to OnTakeAnyDamage Event
	EnemyCharacter->OnTakeAnyDamage.AddDynamic(this, &UEnemyHealth::OnTakeDamage);
}


// Called every frame
void UEnemyHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


// Subscribed to Owner Actor's OnTakeAnyDamage event. Reduces health and checks if enemy is eliminated.
void UEnemyHealth::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, 
	AController* InstigatedBy, AActor* DamageCauser)
{
	if(Health <= 0 || Damage <= 0)
		return;

	// *** Enemy Still Alive
	if (Health > Damage)
		Health -= Damage;

	// *** Enemy Eliminated
	else {
		Health = 0;

		if(EnemyCharacter)
			EnemyCharacter->Destroy();
		else {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EnemyCharacter is null in EnemyHealth"));
		}
	}

	// DEBUG - print current health
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, 
			FString::Printf(TEXT("Enemy Took Damage. Health = %f"), Health));
}

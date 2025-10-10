/*
* Author: Eyan Martucci
* Description: Manages enemy take damage and death
*/

#include "Components/EnemyHealth.h"

#include "Characters/EnemyCharacter.h"		// Enemy Character
#include "Animation/EnemyAnimInstance.h"	// Enemy Anim Instance


// Sets default values for this component's properties
UEnemyHealth::UEnemyHealth()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UEnemyHealth::BeginPlay()
{
	Super::BeginPlay();

	// *** Get References
	EnemyCharacter = Cast<AEnemyCharacter>(GetOwner());
	EnemyAnimInstance = Cast<UEnemyAnimInstance>(EnemyCharacter->GetMesh()->GetAnimInstance());

	// *** Bind functions to events
	EnemyCharacter->OnTakeAnyDamage.AddDynamic(this, &UEnemyHealth::OnTakeDamage);
	EnemyAnimInstance->OnMontageEnded.AddDynamic(this, &UEnemyHealth::OnMontageEnd);
}


// Called every frame
void UEnemyHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// Subscribed to Owner Actor's OnTakeAnyDamage event. Reduces health and checks if enemy is eliminated.
void UEnemyHealth::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, 
	AController* InstigatedBy, AActor* DamageCauser)
{
	if(Health <= 0 || Damage <= 0)
		return;

	// *** Enemy Hurt
	if (Health > Damage) {

		Health -= Damage;
		EnemyAnimInstance->Montage_Play(HurtMontage);			// Play hurt montage

		EnemyCharacter->GetHealthbarWidget()->ShowHealthbar();	// Show healthbar
		EnemyCharacter->GetHealthbarWidget()->SetBarValuePercent(Health / MaxHealth);	// Update healthbar value
	}

	// *** Enemy Death
	else {
		Health = 0;
		EnemyCharacter->StopMovementOnDeath();					// Disable movement
		EnemyAnimInstance->Montage_Play(DeathMontage);			// Start death montage

		EnemyCharacter->GetHealthbarWidget()->HideHealthbar();	// Hide healthbar
	}
}


// Cleanup after montage ends, destroys enemy when death montage is finished
void UEnemyHealth::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted) {

	if (Montage == DeathMontage)					// If death montage ended, destroy enemy
		GetOwner()->Destroy();
	
	if (bInterrupted && EnemyCharacter)				// If montage was interrupted
		EnemyCharacter->DisableAttackCollision();	// Disable attack collision incase attack was interrupted
}	

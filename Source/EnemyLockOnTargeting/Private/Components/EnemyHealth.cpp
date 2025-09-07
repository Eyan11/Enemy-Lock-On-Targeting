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

	if (!GetOwner() && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GetOwner() is null in EnemyHealth"));
		return;
	}

	// *** Get Enemy Character Reference
	EnemyCharacter = Cast<AEnemyCharacter>(GetOwner());
	if (!EnemyCharacter && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EnemyCharacter is null in EnemyHealth"));
		return;
	}

	// *** Get Enemy Anim Instance
	EnemyAnimInstance = Cast<UEnemyAnimInstance>(EnemyCharacter->GetMesh()->GetAnimInstance());
	if (!EnemyAnimInstance && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EnemyAnimInstance is null in EnemyHealth"));
		return;
	}

	if ((!HurtMontage || !DeathMontage) && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("HurtMontage or DeathMontage is null in EnemyHealth"));
		return;
	}

	// *** Subscribe Functions to Events
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

	if ((!EnemyAnimInstance || !EnemyCharacter) && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EnemyAnimInstance or EnemyCharacter is null in EnemyHealth"));
		return;
	}

	// *** Enemy Hurt
	if (Health > Damage) {
		Health -= Damage;
		EnemyAnimInstance->Montage_Play(HurtMontage);

		// *** Update Healthbar
		if (!EnemyCharacter->GetHealthbarWidget() && GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GetHealthbarWidget is null in EnemyHealth"));
			return;
		}
		else {
			EnemyCharacter->GetHealthbarWidget()->ShowHealthbar();
			EnemyCharacter->GetHealthbarWidget()->SetBarValuePercent(Health / MaxHealth);
		}
	}

	// *** Enemy Death
	else {
		Health = 0;
		EnemyCharacter->StopMovementOnDeath();
		EnemyAnimInstance->Montage_Play(DeathMontage);

		// *** Hide Healthbar
		if (!EnemyCharacter->GetHealthbarWidget() && GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GetHealthbarWidget is null in EnemyHealth"));
			return;
		}
		else
			EnemyCharacter->GetHealthbarWidget()->HideHealthbar();
	}

	// DEBUG - print current health
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, 
			FString::Printf(TEXT("Enemy Took Damage. Health = %f"), Health));
}


// Destroy character when death montage is finished
void UEnemyHealth::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted) {

	if (Montage == DeathMontage)
		GetOwner()->Destroy();
	
	if (bInterrupted && EnemyCharacter)
		EnemyCharacter->DisableAttackCollision();
}

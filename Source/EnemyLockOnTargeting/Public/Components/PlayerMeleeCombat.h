/*
* Author: Eyan Martucci
* Description: Manages player attack and take damage
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerMeleeCombat.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENEMYLOCKONTARGETING_API UPlayerMeleeCombat : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerMeleeCombat();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


//*********************************************************


public:

	void OnAttackInput();

	void EnableAttackCollision();
	void DisableAttackCollision();

private:

	UPROPERTY(EditDefaultsOnly, Category = "Animation")	// The animation montage played when doing a normal attack
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")	// The animation montage played when taking damage
	class UAnimMontage* HurtMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")	// The animation montage played when doing a normal attack
	float SwordDamage = 20.0f;

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	class UCapsuleComponent* SwordCollision;

	UPROPERTY()
	class UPlayerAnimInstance* PlayerAnimInstance;

	UPROPERTY()
	bool bIsAttacking = false;

	UPROPERTY()
	bool bCanAttack = true;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);	// Cleans up variables and state when montage finishes

	UFUNCTION()
	void OnSwordBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, 
		AController* InstigatedBy, AActor* DamageCauser);
};

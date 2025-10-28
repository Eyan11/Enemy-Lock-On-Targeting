/*
* Author: Eyan Martucci
* Description: Oversees the entire enemy class, manages movement and attack
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagAssetInterface.h"	// To implement IGameplayTagAssetInterface
#include "UI/EnemyHealthbarWidget.h"	// Enemy Healthbar
#include "EnemyCharacter.generated.h"

// Enemy State Enumeration (Simplified version from EnemyAIController to set movement speed)
UENUM(BlueprintType)
enum class EEnemyMoveState : uint8
{
	Roaming     UMETA(DisplayName = "Roaming"),
	Chasing     UMETA(DisplayName = "Chasing"),
	Retreating  UMETA(DisplayName = "Retreating"),
};


UCLASS()
class ENEMYLOCKONTARGETING_API AEnemyCharacter : public ACharacter, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


//*********************************************************

public:

	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GameplayTags;

	// Required overrides for the interface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override 
		{ TagContainer.AppendTags(GameplayTags); }

	void StartAttacking();
	void SwitchMoveState(EEnemyMoveState newState);
	void EnableAttackCollision();
	void DisableAttackCollision();
	void StopMovementOnDeath();
	bool GetIsInCombat() const { return CurState != EEnemyMoveState::Roaming; }
	UEnemyHealthbarWidget* GetHealthbarWidget() { return HealthbarWidget; }


private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UEnemyHealth* HealthComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UStaticMeshComponent* SwordStaticMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UCapsuleComponent* SwordCollision = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UWidgetComponent* WidgetComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TSubclassOf<UUserWidget> HealthbarWidgetClass;

	UPROPERTY()
	class UEnemyHealthbarWidget* HealthbarWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")	// Auto set in begin play. Controls enemy movement and perception
	class AEnemyAIController* EnemyAIController = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RoamingSpeed = 450.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ChasingSpeed = 670.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RetreatingSpeed = 350.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* AttackMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float SwordDamage = 20.0f;

	UPROPERTY()
	bool bHasAttacked = false;

	UPROPERTY()
	bool bHasDoneDamage = false;

	UPROPERTY()
	class UEnemyAnimInstance* EnemyAnimInstance = nullptr;

	UPROPERTY()
	EEnemyMoveState CurState = EEnemyMoveState::Roaming;

	UFUNCTION()
	void OnSwordBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);
};

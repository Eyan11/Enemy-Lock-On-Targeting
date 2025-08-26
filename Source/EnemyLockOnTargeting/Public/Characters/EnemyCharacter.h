/*
* Author: Eyan Martucci
* Description:
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagAssetInterface.h"	// To implement IGameplayTagAssetInterface
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
	bool GetIsInCombat() { return CurState != EEnemyMoveState::Roaming; }

private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UEnemyHealth* HealthComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")	// Auto set in begin play. Controls enemy movement and perception
	class AEnemyAIController* EnemyAIController = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RoamingSpeed = 450.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ChasingSpeed = 670.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RetreatingSpeed = 350.0f;

	EEnemyMoveState CurState = EEnemyMoveState::Roaming;

};

/*
* Author: Eyan Martucci
* Description:
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagAssetInterface.h"	// To implement IGameplayTagAssetInterface
#include "EnemyCharacter.generated.h"

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
	{
		TagContainer.AppendTags(GameplayTags);
	}

private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UEnemyHealth* HealthComponent = nullptr;

};

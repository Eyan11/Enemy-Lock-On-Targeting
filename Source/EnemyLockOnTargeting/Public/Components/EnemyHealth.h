/*
* Author: Eyan Martucci
* Description:
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyHealth.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENEMYLOCKONTARGETING_API UEnemyHealth : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyHealth();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


//*********************************************************

private:

	UPROPERTY()
	float Health = 100.0f;

	UPROPERTY()
	class AEnemyCharacter* EnemyCharacter = nullptr;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);

};

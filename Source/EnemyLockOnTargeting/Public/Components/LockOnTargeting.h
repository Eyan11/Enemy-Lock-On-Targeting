/*
* Author: Eyan Martucci
* Description: 
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"		// For FGameplayTag UPROPERTY
#include "LockOnTargeting.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENEMYLOCKONTARGETING_API ULockOnTargeting : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULockOnTargeting();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


//*********************************************************

public:

	void StartTargeting();
	void StopTargeting();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")		// Gameplay tag to indicate if an actor is targetable
	FGameplayTag TargetableTag;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")		// The object types that can be targeted
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")		// Actors that will be ignored when checking for targets (self is already added)
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditAnywhere, Category = "Targeting")		// The max targeting distance
	float TargetRadius = 1000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting")	// True if currently targeting an actor
	bool bIsTargeting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting")	// The actor that is being targeted (null if none)
	AActor* targetedActor = nullptr;


private:

	class UCameraComponent* Camera;

	AActor* GetNearestTarget();

};

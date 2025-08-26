/*
* Author: Eyan Martucci
* Description:
*/

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

// Enemy State Enumeration
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	RoamIdle    UMETA(DisplayName = "RoamIdle"),
	Roaming     UMETA(DisplayName = "Roaming"),
	Chasing     UMETA(DisplayName = "Chasing"),
	ChaseIdle	UMETA(DisplayName = "ChaseIdle"),
	Retreating  UMETA(DisplayName = "Retreating"),
	Attacking   UMETA(DisplayName = "Attacking")
};


UCLASS()
class ENEMYLOCKONTARGETING_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	

public:

	AEnemyAIController();
	virtual void Tick(float DeltaTime) override;

	void SwitchEnemyState(EEnemyState NewState);
	void MoveToRandomLocation();
	void ChaseTarget();
	void RetreatFromTarget();

	AActor* GetTargetActor() { return TargetActor; }

protected:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UFUNCTION()
	void OnTargetPerception(AActor* Actor, FAIStimulus Stimulus);

private:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UAIPerceptionComponent* AIPerceptionComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UAISenseConfig_Sight* SightConfigComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float RoamRadius = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SightRadius = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float LoseSightRadius = 2500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")	// The distance from the player that the AI will retreat to
	float RetreatDistance = 800.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")	// The time the AI will continue to follow the target after losing sight
	float TimeUntilLosingSight = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")	// The time the AI waits to roam again after reaching a roam target
	float RoamWaitTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")	// The time the AI waits before chasing target
	float ChaseWaitTime = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float MaxRetreatTime = 6.0f;

	class UNavigationSystemV1* NavSystem = nullptr;
	class AEnemyCharacter* EnemyCharacter = nullptr;
	EEnemyState CurState = EEnemyState::RoamIdle;
	float Timer = 0.0f;
	AActor* TargetActor;
};

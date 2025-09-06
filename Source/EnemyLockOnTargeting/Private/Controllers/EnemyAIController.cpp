/*
* Author: Eyan Martucci
* Description: Manages enemy AI movement target and perception
*/

#include "Controllers/EnemyAIController.h"
#include "Characters/EnemyCharacter.h"			// Enemy Character
#include "NavigationSystem.h"					// Nav Mesh
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionSystem.h"
#include "Characters/PlayerCharacter.h"			// Player Character
#include "Navigation/PathFollowingComponent.h"	// FPathFollowingResult

AEnemyAIController::AEnemyAIController() {

	// *** Create Perception Component
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Comp"));
	if (!AIPerceptionComp && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("AI Perception Component is null in EnemyAIController"));
		return;
	}

	// *** Create Sight Config Component
	SightConfigComp = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	if (!SightConfigComp && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Sight Config Component is null in EnemyAIController"));
		return;
	}

	// *** Setup Sight Config Component
	SightConfigComp->SightRadius = SightRadius;
	SightConfigComp->LoseSightRadius = LoseSightRadius;
	SightConfigComp->PeripheralVisionAngleDegrees = 90.0f;
	SightConfigComp->SetMaxAge(TimeUntilLosingSight);

	SightConfigComp->DetectionByAffiliation.bDetectEnemies = true;
	SightConfigComp->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfigComp->DetectionByAffiliation.bDetectNeutrals = true;

	// *** Setup AI Perception Component
	AIPerceptionComp->ConfigureSense(*SightConfigComp);
	AIPerceptionComp->SetDominantSense(SightConfigComp->GetSenseImplementation());
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerception);
}

void AEnemyAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	// *** Initialize Variables
	SetPawn(InPawn);
	NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	EnemyCharacter = Cast<AEnemyCharacter>(InPawn);

	SwitchEnemyState(EEnemyState::RoamIdle);
}


void AEnemyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (CurState == EEnemyState::RoamIdle) {
		Timer -= DeltaTime;
		if (Timer <= 0)
			SwitchEnemyState(EEnemyState::Roaming);
	}
	else if (CurState == EEnemyState::ChaseIdle) {
		Timer -= DeltaTime;
		if (Timer <= 0)
			SwitchEnemyState(EEnemyState::Chasing);
	}
	else if (CurState == EEnemyState::Retreating) {
		Timer -= DeltaTime;
		if(Timer <= 0)
			SwitchEnemyState(EEnemyState::ChaseIdle);
		else
			RetreatFromTarget();
	}
}


// Sets up the new enemy AI state
void AEnemyAIController::SwitchEnemyState(EEnemyState NewState) {

	CurState = NewState;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("NEW STATE IS: ") + UEnum::GetValueAsString(CurState));

	if (!EnemyCharacter && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("EnemyCharacter is null in EnemyAIController->SwitchEnemyState()"));
		return;
	}

	// *** Setup New State
	switch (CurState) {

		case EEnemyState::RoamIdle:
			Timer = FMath::FRandRange(
				RoamBaseWaitTime - RoamWaitTimeRandomness, RoamBaseWaitTime + RoamWaitTimeRandomness);;
			break;

		case EEnemyState::Roaming:
			MoveToRandomLocation();
			break;

		case EEnemyState::Chasing:
			EnemyCharacter->SwitchMoveState(EEnemyMoveState::Chasing);
			ClearFocus(EAIFocusPriority::Gameplay);
			ChaseTarget();
			break;

		case EEnemyState::ChaseIdle:
			Timer = FMath::FRandRange(
				ChaseBaseWaitTime - ChaseWaitTimeRandomness, ChaseBaseWaitTime + ChaseWaitTimeRandomness);;
			break;

		case EEnemyState::Retreating:
			EnemyCharacter->SwitchMoveState(EEnemyMoveState::Retreating);
			Timer = MaxRetreatTime;
			SetFocus(TargetActor);
			RetreatFromTarget();
			break;

		case EEnemyState::Attacking:
			EnemyCharacter->StartAttacking();
			break;
	}
}


// Called when the acceptance radius is reached for a given move target
void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	Super::OnMoveCompleted(RequestID, Result);

	// Ignore when path is aborted
	if (Result.Code == EPathFollowingResult::Aborted) return;

	if (CurState == EEnemyState::Roaming)
		SwitchEnemyState(EEnemyState::RoamIdle);
	
	else if(CurState == EEnemyState::Chasing)
		SwitchEnemyState(EEnemyState::Attacking);

	else if(CurState == EEnemyState::Retreating)
		SwitchEnemyState(EEnemyState::ChaseIdle);
}


// Called when the target perception updates
void AEnemyAIController::OnTargetPerception(AActor* Actor, FAIStimulus Stimulus) {
	
	if (!Cast<APlayerCharacter>(Actor))	return;		// Only update perception on player

	if (Stimulus.WasSuccessfullySensed()) {			// If found target

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("FOUND TARGET"));

		// *** Start Combat Mode
		TargetActor = Actor;
		SwitchEnemyState(EEnemyState::Chasing);
	}
	else {											// If lost target

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, TEXT("LOST TARGET"));

		if (!EnemyCharacter && GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("EnemyCharacter is null in EnemyAIController->OnTargetPerception()"));
			return;
		}

		// *** Stop Combat Mode
		TargetActor = nullptr;
		SwitchEnemyState(EEnemyState::RoamIdle);
		ClearFocus(EAIFocusPriority::Gameplay);
		EnemyCharacter->SwitchMoveState(EEnemyMoveState::Roaming);
	}
}


// Moves to a random location in the nav mesh bounds within the roam radius
void AEnemyAIController::MoveToRandomLocation() {

	if (!NavSystem && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Navigation System is null in EnemyAIController"));
		return;
	}

	if (!GetPawn() && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GetPawn is null in EnemyAIController"));
		return;
	}

	// *** Move to Random Location
	FNavLocation result;
	if (NavSystem->GetRandomReachablePointInRadius(GetPawn()->GetActorLocation(), RoamRadius, result)) {
		MoveToLocation(result);
	}
}


// Moves towards player until acceptance radius is reached or sight of player is lost
void AEnemyAIController::ChaseTarget() {
	if (!TargetActor && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TargetActor is null in EnemyAIController->ChaseTarget()"));
		return;
	}

	MoveToActor(TargetActor, 100.0f);
}


// Moves away from player until a certain distance is reached
void AEnemyAIController::RetreatFromTarget() {

	if (!GetPawn() && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GetPawn() is null in EnemyAIController->RetreatFromTarget()"));
		return;
	}
	if (!TargetActor && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TargetActor is null in EnemyAIController->RetreatFromTarget()"));
		return;
	}
	
	// Get direction away from target
	FVector retreatDirection = 
		(GetPawn()->GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal2D();

	FVector retreatLocation = TargetActor->GetActorLocation() + (retreatDirection * RetreatDistance);

	MoveToLocation(retreatLocation);
}


// Called when attack montage is finished
void AEnemyAIController::OnFinishAttack() {

	// If still in attack state (enemy could lose sight of player while attacking and start roaming)
	if (CurState == EEnemyState::Attacking)
		SwitchEnemyState(EEnemyState::Retreating);
}
/*
* Author: Eyan Martucci
* Description: Manages enemy animations
*/

#include "Animation/EnemyAnimInstance.h"

#include "Characters/EnemyCharacter.h"				// Enemy Character
#include "GameFramework/PawnMovementComponent.h"	// IsFalling check


void UEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeInitializeAnimation();		// Still run event for parent class

	APawn* PawnOwner = TryGetPawnOwner();
	EnemyCharacter = Cast<AEnemyCharacter>(PawnOwner);
}


void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);		// Still run event for parent class

	if (!EnemyCharacter) return;

	FVector velocity = EnemyCharacter->GetVelocity();
	ForwardSpeed = FVector::DotProduct(EnemyCharacter->GetActorForwardVector(), velocity);

	bIsFalling = EnemyCharacter->GetMovementComponent()->IsFalling();
	bIsInCombat = EnemyCharacter->GetIsInCombat();
}

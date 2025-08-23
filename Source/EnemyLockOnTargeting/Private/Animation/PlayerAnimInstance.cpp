/*
* Author: Eyan Martucci
* Description: Manages player animations
*/

#include "Animation/PlayerAnimInstance.h"

#include "GameFramework/Character.h"				// For Character ref
#include "Characters/PlayerCharacter.h"				// For APlayerCharacter
#include "GameFramework/PawnMovementComponent.h"	// For IsFalling check


void UPlayerAnimInstance::NativeBeginPlay() {
	Super::NativeInitializeAnimation();		// Still run event for parent class

	APawn* PawnOwner = TryGetPawnOwner();

	// DEBUG
	if (!PawnOwner && GEngine) {
		GEngine->AddOnScreenDebugMessage(
			-1, 5.0f, FColor::Red,
			TEXT("PawnOwner Reference is null in PlayerAnimInstance.cpp"));
		return;
	}

	PlayerCharacter = Cast<APlayerCharacter>(PawnOwner);

	// DEBUG
	if (!PlayerCharacter && GEngine) {
		GEngine->AddOnScreenDebugMessage(
			-1, 5.0f, FColor::Red,
			TEXT("Player Reference is null in PlayerAnimInstance.cpp"));
		return;
	}
}

/* Gets player speed and if grounded */
void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);		// Still run event for parent class
	
	if (!PlayerCharacter) return;

	FVector playerVel = PlayerCharacter->GetVelocity();
	Speed = playerVel.Size();
	ForwardSpeed = FVector::DotProduct(PlayerCharacter->GetActorForwardVector(), playerVel);
	RightSpeed = FVector::DotProduct(PlayerCharacter->GetActorRightVector(), playerVel);

	bIsFalling = PlayerCharacter->GetMovementComponent()->IsFalling();
	bIsTargeting = PlayerCharacter->IsTargetingInputHeld();
}
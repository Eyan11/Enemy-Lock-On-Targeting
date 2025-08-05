/*
* Author: Eyan Martucci
* Description: Manages player animations
*/

#include "Amimation/PlayerAnimInstance.h"

#include "GameFramework/Character.h"				// For Character ref
#include "Characters/PlayerCharacter.h"				// For APlayerCharacter
#include "GameFramework/PawnMovementComponent.h"	// For IsFalling check


/* Initializes references */
void UPlayerAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();		// Still run event for parent class

	APawn* PawnOwner = TryGetPawnOwner();

	// DEBUG
	if (!PawnOwner && GEngine) {
		GEngine->AddOnScreenDebugMessage(
			-1, 5.0f, FColor::Red,
			TEXT("PawnOwner Reference is null in PlayerAnimInstance.cpp"));
		return;
	}
	
	Player = Cast<APlayerCharacter>(PawnOwner);

	// DEBUG
	if (!Player && GEngine) {
		GEngine->AddOnScreenDebugMessage(
			-1, 5.0f, FColor::Red,
			TEXT("Player Reference is null in PlayerAnimInstance.cpp"));
		return;
	}
}


/* Gets player speed and if grounded */
void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);		// Still run event for parent class
	
	if (!Player) return;

	FVector playerVel = Player->GetVelocity();
	Speed = playerVel.Size();
	VerticalSpeed = FVector::DotProduct(Player->GetActorForwardVector(), playerVel);
	HorizontalSpeed = FVector::DotProduct(Player->GetActorRightVector(), playerVel);

	bIsFalling = Player->GetMovementComponent()->IsFalling();
	bIsTargeting = Player->IsTargetingInputHeld();
}
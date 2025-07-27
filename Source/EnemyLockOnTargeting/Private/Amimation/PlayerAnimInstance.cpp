// Fill out your copyright notice in the Description page of Project Settings.


#include "Amimation/PlayerAnimInstance.h"

#include "GameFramework/Character.h"				// For Character ref
#include "GameFramework/PawnMovementComponent.h"	// For IsFalling check


/* Initializes references */
void UPlayerAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();		// Still run event for parent class

	APawn* PawnOwner = TryGetPawnOwner();
	if (!PawnOwner)
		return;
	
	Character = Cast<ACharacter>(PawnOwner);
	if (!Character)
		return;
}


/* Gets player speed and if grounded */
void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);		// Still run event for parent class
	
	if (!Character) {
		// print error message
		UE_LOG(LogTemp, Error, TEXT("PlayerAnimInstance: Character is null in NativeUpdateAnimation"));
		return;
	}

	Speed = Character->GetVelocity().Size();
	bIsFalling = Character->GetMovementComponent()->IsFalling();
}
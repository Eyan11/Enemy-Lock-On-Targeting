// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerMeleeCombat.h"

#include "Characters/PlayerCharacter.h"

// Sets default values for this component's properties
UPlayerMeleeCombat::UPlayerMeleeCombat()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

}


// Called when the game starts
void UPlayerMeleeCombat::BeginPlay()
{
	Super::BeginPlay();

	
	// *** Get References
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());


	// DEBUG
	if (PlayerCharacter && NormalAttackMontage) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Got all references in PlayerMeleeCombat"));
	}
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Missing a reference in PlayerMeleeCombat"));
	}
}


// Called every frame
void UPlayerMeleeCombat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


// Starts attack montage and hitbox
void UPlayerMeleeCombat::OnAttackInput() {

	if (!PlayerCharacter) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("PlayerCharacter is null in PlayerMeleeCombat"));
		return;
	}

	// *** Get Anim Instance
	UAnimInstance* animInstance = PlayerCharacter->GetMesh()->GetAnimInstance();
	if (!animInstance) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("animInstance is null in PlayerMeleeCombat"));
		return;
	}

	// *** Play Attack Montage
	if (!bIsAttacking && !animInstance->Montage_IsPlaying(NormalAttackMontage)) {

		animInstance->Montage_Play(NormalAttackMontage);
		bIsAttacking = true;
		PlayerCharacter->StopMovement();
		animInstance->OnMontageEnded.AddDynamic(this, &UPlayerMeleeCombat::OnAttackMontageEnded);
	}
}


// Cleans up attack montage variables
void UPlayerMeleeCombat::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) {

	bIsAttacking = false;
	PlayerCharacter->ResumeMovement();
}

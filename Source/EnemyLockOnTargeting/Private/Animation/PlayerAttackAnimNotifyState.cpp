/*
* Author: Eyan Martucci
* Description: Notifies player melee combat component when to enable and disable weapon collision
*/

#include "Animation/PlayerAttackAnimNotifyState.h"

#include "Characters/PlayerCharacter.h"		// For APlayerCharacter
#include "Components/PlayerMeleeCombat.h"	// For UPlayerMeleeCombat


void UPlayerAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation,float TotalDuration, const FAnimNotifyEventReference& EventReference) 
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);


	// *** Get Melee Combat Component Reference
	if (!MeshComp || !MeshComp->GetOwner()) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No MeshComp or MeshComp->GetOwner()"));
		return;
	}

	APlayerCharacter* PlayerCharacter =
		Cast<APlayerCharacter>(MeshComp->GetOwner());

	if (!PlayerCharacter) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No PlayerCharacter"));
		return;
	}

	class UPlayerMeleeCombat* MeleeCombatComp = PlayerCharacter->GetMeleeCombatComponent();
	if (!MeleeCombatComp) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No MeleeCombatComp"));
		return;
	}

	// *** Enable Attack Collision
	MeleeCombatComp->EnableAttackCollision();
}

void UPlayerAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) 
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	
	// *** Get Melee Combat Component Reference
	if (!MeshComp || !MeshComp->GetOwner()) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No MeshComp or MeshComp->GetOwner()"));
		return;
	}

	APlayerCharacter* PlayerCharacter =
		Cast<APlayerCharacter>(MeshComp->GetOwner());

	if (!PlayerCharacter) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No PlayerCharacter"));
		return;
	}

	class UPlayerMeleeCombat* MeleeCombatComp = PlayerCharacter->GetMeleeCombatComponent();
	if (!MeleeCombatComp) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No MeleeCombatComp"));
		return;
	}
	
	// *** Disable Attack Collision
	MeleeCombatComp->DisableAttackCollision();
}


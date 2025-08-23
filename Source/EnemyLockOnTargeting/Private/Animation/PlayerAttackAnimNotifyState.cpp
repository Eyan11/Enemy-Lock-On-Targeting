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

	// *** Check Melee Combat Component Reference
	if (!IsValid(MeleeCombatComp)) {

		if (!MeshComp || !MeshComp->GetOwner()) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No MeshComp or MeshComp->GetOwner()"));
			return;
		}

		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (!PlayerCharacter) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No PlayerCharacter reference in PlayerAttackAnimNotifyState"));
			return;
		}

		MeleeCombatComp = PlayerCharacter->GetMeleeCombatComponent();
		if (!MeleeCombatComp) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No MeleeCombatComp reference in PlayerAttackAnimNotifyState"));
			return;
		}
	}

	// *** Enable Attack Collision
	MeleeCombatComp->EnableAttackCollision();
}

void UPlayerAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) 
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	// *** Check Melee Combat Component Reference
	if (!IsValid(MeleeCombatComp)) {

		if (!MeshComp || !MeshComp->GetOwner()) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No MeshComp or MeshComp->GetOwner()"));
			return;
		}

		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (!PlayerCharacter) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No PlayerCharacter reference in PlayerAttackAnimNotifyState"));
			return;
		}

		MeleeCombatComp = PlayerCharacter->GetMeleeCombatComponent();
		if (!MeleeCombatComp) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No MeleeCombatComp reference in PlayerAttackAnimNotifyState"));
			return;
		}
	}
	
	// *** Disable Attack Collision
	MeleeCombatComp->DisableAttackCollision();
}


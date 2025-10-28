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
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
		MeleeCombatComp = PlayerCharacter->GetMeleeCombatComponent();
	}

	// *** Enable Attack Collision
	if (IsValid(MeleeCombatComp))
		MeleeCombatComp->EnableAttackCollision();
}

void UPlayerAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) 
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	// *** Check Melee Combat Component Reference
	if (!IsValid(MeleeCombatComp)) {
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
		MeleeCombatComp = PlayerCharacter->GetMeleeCombatComponent();
	}
	
	// *** Disable Attack Collision
	if(IsValid(MeleeCombatComp))
		MeleeCombatComp->DisableAttackCollision();
}


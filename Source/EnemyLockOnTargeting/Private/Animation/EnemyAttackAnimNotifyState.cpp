/*
* Author: Eyan Martucci
* Description: Notifies EnemyCharacter when to enable and disable weapon collision
*/

#include "Animation/EnemyAttackAnimNotifyState.h"

#include "Characters/EnemyCharacter.h"		// EnemyCharacter


void UEnemyAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// *** Check Enemy Character Reference
	if (!IsValid(EnemyCharacter))
		EnemyCharacter = Cast<AEnemyCharacter>(MeshComp->GetOwner());

	// *** Enable Attack Collision
	EnemyCharacter->EnableAttackCollision();
}

void UEnemyAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	// *** Check Enemy Character Reference
	if (!IsValid(EnemyCharacter))
		EnemyCharacter = Cast<AEnemyCharacter>(MeshComp->GetOwner());

	// *** Disable Attack Collision
	EnemyCharacter->DisableAttackCollision();
}
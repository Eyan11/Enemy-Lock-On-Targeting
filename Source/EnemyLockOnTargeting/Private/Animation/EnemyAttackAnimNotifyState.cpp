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
	if (!IsValid(EnemyCharacter)) {

		if (!MeshComp || !MeshComp->GetOwner()) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No MeshComp or MeshComp->GetOwner() in EnemyAttackAnimNotifyState"));
			return;
		}

		EnemyCharacter = Cast<AEnemyCharacter>(MeshComp->GetOwner());
		if (!EnemyCharacter) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No EnemyCharacter reference in EnemyAttackAnimNotifyState"));
			return;
		}
	}

	// *** Enable Attack Collision
	EnemyCharacter->EnableAttackCollision();
}

void UEnemyAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	// *** Check Enemy Character Reference
	if (!IsValid(EnemyCharacter)) {

		if (!MeshComp || !MeshComp->GetOwner()) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No MeshComp or MeshComp->GetOwner() in EnemyAttackAnimNotifyState"));
			return;
		}

		EnemyCharacter = Cast<AEnemyCharacter>(MeshComp->GetOwner());
		if (!EnemyCharacter) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No EnemyCharacter reference in EnemyAttackAnimNotifyState"));
			return;
		}
	}

	// *** Disable Attack Collision
	EnemyCharacter->DisableAttackCollision();
}
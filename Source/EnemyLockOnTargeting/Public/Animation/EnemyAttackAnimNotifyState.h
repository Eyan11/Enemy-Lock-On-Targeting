/*
* Author: Eyan Martucci
* Description: Notifies EnemyCharacter when to enable and disable weapon collision
*/

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "EnemyAttackAnimNotifyState.generated.h"


UCLASS()
class ENEMYLOCKONTARGETING_API UEnemyAttackAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

private:

	UPROPERTY()
	class AEnemyCharacter* EnemyCharacter = nullptr;

};

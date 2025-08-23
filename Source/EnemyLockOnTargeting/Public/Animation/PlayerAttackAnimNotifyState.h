/*
* Author: Eyan Martucci
* Description: Notifies player melee combat component when to enable and disable weapon collision
*/

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PlayerAttackAnimNotifyState.generated.h"


UCLASS()
class ENEMYLOCKONTARGETING_API UPlayerAttackAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	

public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference) override;

private:

	UPROPERTY()
	class UPlayerMeleeCombat* MeleeCombatComp = nullptr;

};

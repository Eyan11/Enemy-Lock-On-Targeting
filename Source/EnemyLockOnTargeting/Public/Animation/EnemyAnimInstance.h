/*
* Author: Eyan Martucci
* Description: Manages enemy animations
*/

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class ENEMYLOCKONTARGETING_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	// Override the animation begin play method
	virtual void NativeBeginPlay() override;

	// Override the animation tick method
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:

	// References
	UPROPERTY()
	class AEnemyCharacter* EnemyCharacter = nullptr;

	// Movement Variables
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// Magnitude of the player's 3D velocity
	float ForwardSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// True when player is in the air
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))	// True when player is holding targeting input
	bool bIsInCombat = false;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * Controls player animations
 */

UCLASS()
class ENEMYLOCKONTARGETING_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Override the animation begin play method
	virtual void NativeInitializeAnimation() override;

	// Override the animation tick method
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	// References
	UPROPERTY()
	class ACharacter* Character = nullptr;

	// Movement Variables
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsFalling = false;
	
};

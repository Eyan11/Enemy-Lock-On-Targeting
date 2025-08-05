/*
* Author: Eyan Martucci
* Description: Manages player animations
*/

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"


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
	class APlayerCharacter* Player = nullptr;

	// Movement Variables
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float HorizontalSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float VerticalSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting", meta = (AllowPrivateAccess = "true"))
	bool bIsTargeting = false;
	
};

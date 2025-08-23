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
	virtual void NativeBeginPlay() override;

	// Override the animation tick method
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:

	// References
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter = nullptr;

	// Movement Variables
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// Magnitude of the player's 3D velocity
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// Speed of player relative to player right direction (pos is right, neg is left)
	float RightSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// Speed of player relative to player forward direction (pos is forward, neg is backwards)
	float ForwardSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))	// True when player is in the air
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting", meta = (AllowPrivateAccess = "true"))	// True when player is holding targeting input
	bool bIsTargeting = false;
	
};

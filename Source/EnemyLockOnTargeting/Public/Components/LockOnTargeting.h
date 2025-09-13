/*
* Author: Eyan Martucci
* Description: Manages the lock on targeting system which checks for nearby targets
*	and makes sure the player and target are in view of the camera.
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"		// For FGameplayTag UPROPERTY
#include "LockOnTargeting.generated.h"

class ATargetingArrow;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENEMYLOCKONTARGETING_API ULockOnTargeting : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULockOnTargeting();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


//*********************************************************

public:

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")	// True if currently targeting an actor
	bool bIsTargeting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")	// True if camera is rotating to player forward direction
	bool bIsCameraResetting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")	// The actor that is being targeted (null if none)
	AActor* TargetedActor = nullptr;


	void OnTargetingInputStart();			// Starts either targeting or camera reset
	void OnTargetingInputEnd();				// Cancels the lock on the current target
	void OnSwitchDirectionalTargetInput(bool bGetRight);	// Switches targets to the next closest target on the left or right
	void OnLookInput(FVector2D LookInput);	// Checks to stop camera reset or adjust targeting offset angle


private:

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")	// Gameplay tag to indicate if an actor is targetable
	FGameplayTag TargetableTag;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")	// The object types that can be targeted
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")	// Actors that will be ignored when checking for targets (self is already added)
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")	// The max distance between the player and target
	float MaxTargetingDistance = 2500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")	// The speed at which the spring arm moves to its target
	float SpringArmInterpSpeed = 8.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")	// The time frame from releasing to pressing the targeting button will get a different target
	float SwitchTargetsTimeFrame = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float UpdateNonTargetingInterval = 0.5;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")	// Reference to BP_TargetingArrow so the blueprint subclass can be spawned
	TSubclassOf<ATargetingArrow> TargetingArrowClass;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")	// The speed at which the camera rotates to its target
	float CameraRotationSpeed = 8.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")	// The speed of camera rotation from input when targeting
	float TargetingCameraSensitivity = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")	// The angle relative to player to offset the camera when entering targeting mode
	float DefaultTargetingYawOffset = -30.0f;

	UPROPERTY()
	class USpringArmComponent* SpringArm;
	UPROPERTY()
	class UCameraComponent* Camera;
	UPROPERTY()
	class APlayerController* PlayerController;
	UPROPERTY()
	ATargetingArrow* TargetingArrow;	// 2D arrow above targeted actor
	UPROPERTY()
	AActor* PlayerActor;
	UPROPERTY()
	AActor* PreviousTargetedActor;		// The last actor to be targeted
	UPROPERTY()
	AActor* NonTargetingActor;			// The actor with an arrow overhead in non targeting mode
	UPROPERTY()
	float DefaultSpringArmLength;		// The initial spring arm target arm length set in player blueprint

	FRotator TargetRotation;			// The current target rotation for either targeting or camera reset
	FRotator TargetingOffsetRotation;	// The offset from player rotation that the spring arm is when targeting
	float SwitchTargetsTimer;			// Tracks time after releasing the targeting button
	float UpdateNonTargetingTimer;		// Tracks time left until UpdateNonTargeting method is called
	bool bIsCleaningUpTargeting = false;// True if spring arm is still returning to default values after targeting is over
	bool bCanSwitchTargets = false;		// True if the player can get a different target when pressing the switch target button

	TArray<AActor*> GetAllTargetsInRange();		// Returns all actors with targetable tag within sphere overlap
	AActor* GetNearestTarget(bool bConsiderPreviousTarget);	// Returns closest targetable actor in proximity
	AActor* GetNextTargetInDirection(bool bCheckRight);	// Returns the next closest target to the left or right of current target
	void UpdateCameraReset(float DeltaTime);	// Rotates camera to player forward direction
	void UpdateTargeting();						// Updates spring arm and camera to keep player and enemy in view
	void UpdateTargetingCleanup();				// Updates spring arm to return to default values after targeting is over
	void UpdateNonTargeting();					// Sets arrow sprite above closest target in range
};

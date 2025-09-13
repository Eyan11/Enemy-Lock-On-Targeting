/*
* Author: Eyan Martucci
* Description: Manages the lock on targeting system which checks for nearby targets
*	and makes sure the player and target are in view of the camera.
*/

#include "Components/LockOnTargeting.h"

#include "Kismet/KismetSystemLibrary.h"			// For SphereOverlapActors
#include "GameplayTagAssetInterface.h"			// For IGameplayTagAssetInterface
#include "GameplayTagContainer.h"				// For FGameplayTag
#include "GameFramework/Actor.h"				// For AActor
#include "Engine/World.h"						// For GetWorld()
#include "GameFramework/SpringArmComponent.h"	// For Spring Arm
#include "Camera/CameraComponent.h"				// For Camera
#include "Kismet/GameplayStatics.h"				// For GetPlayerController
#include "UI/TargetingArrow.h"					// For TargetingArrow Actor

// Sets default values for this component's properties
ULockOnTargeting::ULockOnTargeting()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	// *** Initialize Variables
	TargetingOffsetRotation = FRotator::ZeroRotator;
	TargetingOffsetRotation.Yaw = DefaultTargetingYawOffset;
	SwitchTargetsTimer = SwitchTargetsTimeFrame;
}


// Called when the game starts
void ULockOnTargeting::BeginPlay()
{
	Super::BeginPlay();

	// *** Get References
	PlayerActor = GetOwner();
	ActorsToIgnore.Add(PlayerActor);	// Don't target self
	SpringArm = PlayerActor->FindComponentByClass<USpringArmComponent>();
	Camera = PlayerActor->FindComponentByClass<UCameraComponent>();
	DefaultSpringArmLength = SpringArm->TargetArmLength;
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// DEBUG
	if (PlayerActor && SpringArm && Camera && PlayerController && TargetingArrowClass && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Got all references in LockOnTargeting"));
	}
	else if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Missing a reference in LockOnTargeting"));
	}


	// *** Spawn Targeting Arrow
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	TargetingArrow = GetWorld()->SpawnActor<ATargetingArrow>(TargetingArrowClass, 
		FVector::ZeroVector, FRotator::ZeroRotator,SpawnParams);

	// DEBUG
	if (TargetingArrow && GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Spawned Targeting Arrow"));
	else if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Did Not Spawn Targeting Arrow"));
}


// Called every frame
void ULockOnTargeting::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	// *** Update Lock on Targeting
	if (bIsTargeting)
		UpdateTargeting();

	// *** Update Spring Arm to Default Values
	else if (bIsCleaningUpTargeting)
		UpdateTargetingCleanup();
	
	// *** Update Camera Reset
	else if (bIsCameraResetting)
		UpdateCameraReset(DeltaTime);

	// *** Reduce Switch Targets Timer
	if (bCanSwitchTargets) {
		SwitchTargetsTimer -= DeltaTime;

		if (SwitchTargetsTimer <= 0) {		// If timer expires, don't allow target switching
			bCanSwitchTargets = false;
			PreviousTargetedActor = nullptr;
		}
	}

	// *** Call UpdateNonTargeting on Interval
	if (!bIsTargeting) {
		UpdateNonTargetingTimer -= DeltaTime;

		if (UpdateNonTargetingTimer <= 0) {
			UpdateNonTargeting();
			UpdateNonTargetingTimer = UpdateNonTargetingInterval;
		}
	}
}


// Checks for nerest target, then either starts targeting or starts resetting camera
void ULockOnTargeting::OnTargetingInputStart() {

	// *** Get Nearest Actor With Targetable Tag
	TargetedActor = GetNearestTarget(true);

	// *** Start Lock On Targeting
	if (TargetedActor) {			// If there is a target to lock onto

		// Check to flip camera Yaw
		float dotProdResult = FVector::DotProduct
			(PlayerActor->GetActorRightVector(), Camera->GetForwardVector());

		if (dotProdResult > 0)		// If camera is on left side of player (acute angle)
			TargetingOffsetRotation.Yaw = -DefaultTargetingYawOffset;	// Flip default yaw to left side

		// Initialize variables
		bIsTargeting = true;
		bIsCleaningUpTargeting = false;
		TargetingArrow->SetTarget(TargetedActor);
		TargetingArrow->StartTargetingMode();
	}

	// *** Start Camera Reset
	else {							// If there is no target to lock onto
		TargetRotation = PlayerActor->GetActorRotation();
		bIsCameraResetting = true;
	}
}


// Sets target to the next closest target to the left or right of current one
void ULockOnTargeting::OnSwitchDirectionalTargetInput(bool bGetRight) {

	TargetedActor = GetNextTargetInDirection(bGetRight);
	TargetingArrow->SetTarget(TargetedActor);
	TargetingArrow->StartTargetingMode();
}


// Updates Spring arm offset to keep player and target centered and in view
void ULockOnTargeting::UpdateTargeting() {
	
	// *** Check to Stop Targeting
	if (!IsValid(TargetedActor) || SpringArm->TargetArmLength > MaxTargetingDistance) {
		OnTargetingInputEnd();
		return;
	}
	
	// *** Update Spring Arm Target Offset
	FVector targetOffset = (TargetedActor->GetActorLocation() - PlayerActor->GetActorLocation()) / 2;

	FVector interpolatedOffset = FMath::VInterpTo(SpringArm->TargetOffset,
		targetOffset, GetWorld()->GetDeltaSeconds(), SpringArmInterpSpeed);

	SpringArm->TargetOffset = interpolatedOffset;


	// *** Update Spring Arm Length
	float targetLength = 500 + targetOffset.Length();

	float interpolatedLength = FMath::FInterpTo(SpringArm->TargetArmLength,
		targetLength, GetWorld()->GetDeltaSeconds(), SpringArmInterpSpeed);

	SpringArm->TargetArmLength = interpolatedLength;
	

	// *** Update Camera Rotation
	TargetRotation = TargetingOffsetRotation;
	TargetRotation.Yaw += PlayerActor->GetActorRotation().Yaw;

	FRotator interpolatedRot = FMath::RInterpTo(PlayerController->GetControlRotation(),
		TargetRotation, GetWorld()->GetDeltaSeconds(), CameraRotationSpeed);

	PlayerController->SetControlRotation(interpolatedRot);
}


// Smoothly updates spring arm target offset and target arm length back to default values
void ULockOnTargeting::UpdateTargetingCleanup() {

	// *** Checked if Reached Target
	if (SpringArm->TargetOffset.IsNearlyZero(0.1) &&
		SpringArm->TargetArmLength < DefaultSpringArmLength + 0.1) {

		SpringArm->TargetOffset = FVector::ZeroVector;
		SpringArm->TargetArmLength = DefaultSpringArmLength;
		bIsCleaningUpTargeting = false;
		return;
	}

	// *** Update Spring Arm Target Offset
	FVector interpolatedOffset = FMath::VInterpTo(SpringArm->TargetOffset,
		FVector::ZeroVector, GetWorld()->GetDeltaSeconds(), SpringArmInterpSpeed);

	SpringArm->TargetOffset = interpolatedOffset;


	// *** Update Spring Arm Length
	float interpolatedLength = FMath::FInterpTo(SpringArm->TargetArmLength,
		DefaultSpringArmLength, GetWorld()->GetDeltaSeconds(), SpringArmInterpSpeed);

	SpringArm->TargetArmLength = interpolatedLength;
}

// Sets arrow sprite over the head of the nearest enemy in range
void ULockOnTargeting::UpdateNonTargeting() {

	AActor* nearestTarget = GetNearestTarget(false);

	if (!nearestTarget) {
		NonTargetingActor = nullptr;
		TargetingArrow->HideArrow();
	}
	else if (NonTargetingActor != nearestTarget) {
		NonTargetingActor = nearestTarget;
		TargetingArrow->SetTarget(NonTargetingActor);
		TargetingArrow->StartNonTargetingMode();
	}
}


// Stops targeting and resets variables
void ULockOnTargeting::OnTargetingInputEnd() {

	bIsTargeting = false;
	bIsCleaningUpTargeting = true;		// Smoothly update spring arm to default values
	bCanSwitchTargets = true;
	SwitchTargetsTimer = SwitchTargetsTimeFrame;
	UpdateNonTargetingTimer = UpdateNonTargetingInterval;
	NonTargetingActor = nullptr;

	if (IsValid(TargetedActor))
		PreviousTargetedActor = TargetedActor;
	else
		PreviousTargetedActor = nullptr;

	TargetedActor = nullptr;

	TargetingOffsetRotation = FRotator::ZeroRotator;
	TargetingOffsetRotation.Yaw = DefaultTargetingYawOffset;

	// Handle Arrow
	UpdateNonTargeting();
}


// Returns all actors with targetable tag in range of the sphere overlap
TArray<AActor*> ULockOnTargeting::GetAllTargetsInRange() {
	TArray<AActor*> nearbyActors;

	// *** Get All Actors in Sphere Overlap
	float sphereRadius = MaxTargetingDistance / 2.0f;
	FVector camForward2D = Camera->GetForwardVector().GetSafeNormal2D();

	FVector sphereStart = PlayerActor->GetActorLocation() + (camForward2D * sphereRadius);

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), sphereStart,
		sphereRadius, ObjectTypes, nullptr, ActorsToIgnore, nearbyActors);

	// DEBUG
	#if WITH_EDITOR								// Only draw in editor, not in final build
	DrawDebugSphere(GetWorld(), sphereStart,
			sphereRadius, 15, FColor::Green, false, 1.0f, 0, 1.5f);
	#endif


	TArray<AActor*> targetableActors;

	// *** Find All Actors in Overlap With Targetable Tag
	for (AActor* actor : nearbyActors) {

		// *** Check if Implementing Gameplay Tags Interface
		IGameplayTagAssetInterface* tagInterface = Cast<IGameplayTagAssetInterface>(actor);
		if (!tagInterface) continue;

		// *** Check if Actor has Targetable Tag
		if (tagInterface->HasMatchingGameplayTag(TargetableTag))
			targetableActors.Add(actor);
	}

	return targetableActors;
}


// Returns the closest actor with targetable gameplay tag in sphere overlap. 
//	If switching targets by distance instead of direction, then it gets the next closest actor.
AActor* ULockOnTargeting::GetNearestTarget(bool bConsiderPreviousTarget) {

	// *** Get All Actors With Targetable Tag in Sphere Overlap
	TArray<AActor*> targetableActors = GetAllTargetsInRange();
	bool bRemovedPreviousTarget = false;

	// *** Check if Switching Targets
	if (bConsiderPreviousTarget && bCanSwitchTargets &&
		PreviousTargetedActor && targetableActors.Contains(PreviousTargetedActor)) 
	{
		targetableActors.Remove(PreviousTargetedActor);
		bRemovedPreviousTarget = true;
	}

	// *** Initialize Variables
	AActor* closestActor = nullptr;										// Current closest actor in loop
	float closestDist = MaxTargetingDistance * MaxTargetingDistance;	// Distance to closest actor
	float curDist = FLT_MAX;											// Distance of current actor in loop

	// *** Find Closest Target
	for(AActor* actor : targetableActors) {

		curDist = actor->GetSquaredDistanceTo(PlayerActor);

		if (curDist < closestDist) {	// Found new closest actor
			closestDist = curDist;
			closestActor = actor;
		}
	}

	// *** Return Next Target
	if (!closestActor && bRemovedPreviousTarget)	// If previous target is the only nearby targetable actor	
		return PreviousTargetedActor;
	else											// If there is another targetable actor besides previous target
		return closestActor;
}


// Returns the next closest target to the left or right of the current target
AActor* ULockOnTargeting::GetNextTargetInDirection(bool bCheckRight) {

	// *** Get All Actors With Targetable Tag in Sphere Overlap
	TArray<AActor*> targetableActors = GetAllTargetsInRange();

	// *** Ignore Current Actor
	if (TargetedActor && targetableActors.Contains(TargetedActor))
		targetableActors.Remove(TargetedActor);

	// *** Initialize Variables
	float curDotProd;											// Dot product of the current actor in loop
	AActor* closestActor = nullptr;								// Current closest actor
	float closestDotProd = bCheckRight ? FLT_MAX : -FLT_MAX;	// Dot product of closest actor

	// DEBUG
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.0f, FColor::Blue,
			TEXT("Number of targetable actors = ") + FString::FromInt(targetableActors.Num()));
	}

	// *** Find Closest Target
	for (AActor* actor : targetableActors) {

		// *** Get Dot Product Between Player Right and Target Direction
		curDotProd = FVector::DotProduct
			(PlayerActor->GetActorRightVector(), actor->GetActorLocation() - PlayerActor->GetActorLocation());
		
		// *** Check if Target is Next Closest to the Right
		if (bCheckRight && curDotProd > 0 && curDotProd < closestDotProd) {
			closestActor = actor;
			closestDotProd = curDotProd;
		}

		// *** Check if Target is Next Closest to the Left
		else if (!bCheckRight && curDotProd < 0 && curDotProd > closestDotProd) {
			closestActor = actor;
			closestDotProd = curDotProd;
		}
	}

	// *** Return Next Target
	if (!closestActor)
		return TargetedActor;
	else
		return closestActor;
}


// Rotates the camera smoothly to face player forward direction
void ULockOnTargeting::UpdateCameraReset(float DeltaTime) {

	// *** Check if Reached Target Rotation
	FRotator curRot = PlayerController->GetControlRotation();
	if (curRot.Equals(TargetRotation, 1.0)) {
		bIsCameraResetting = false;
		return;
	}

	// *** Get Smoothed Rotation to Target
	FRotator interpolatedRot = FMath::RInterpTo(curRot,
		TargetRotation, GetWorld()->GetDeltaSeconds(), CameraRotationSpeed);

	// *** Apply Rotation
	PlayerController->SetControlRotation(interpolatedRot);
}


// Checks to cancel camera reset or adjust targeting offset angle
void ULockOnTargeting::OnLookInput(FVector2D LookInput) {

	// *** Check For Camera Reset Interrupt
	if (bIsCameraResetting && LookInput.SquaredLength() > 0.02)
		bIsCameraResetting = false;
	
	// *** Check For Targeting Offset Rotation Adjustment
	else if (bIsTargeting) {
		// Make new input look apart of the offset
		TargetingOffsetRotation.Yaw += LookInput.X * TargetingCameraSensitivity;
		TargetingOffsetRotation.Pitch += LookInput.Y * TargetingCameraSensitivity;

		// Clamp camera pitch between -90 and 90 degrees
		TargetingOffsetRotation.Pitch = FMath::Clamp(TargetingOffsetRotation.Pitch, -89.999f, 89.999f);
	}
}


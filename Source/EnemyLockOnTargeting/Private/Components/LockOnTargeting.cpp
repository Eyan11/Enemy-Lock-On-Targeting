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
#include "GameFramework/SpringArmComponent.h"	// Spring Arm
#include "Kismet/GameplayStatics.h"				// For GetPlayerController

// Sets default values for this component's properties
ULockOnTargeting::ULockOnTargeting()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	// *** Initialize Variables
	PlayerActor = GetOwner();
	ActorsToIgnore.Add(PlayerActor);	// Don't target self
	TargetingOffsetRotation.Yaw = DefaultTargetingOffsetAngle;
}


// Called when the game starts
void ULockOnTargeting::BeginPlay()
{
	Super::BeginPlay();

	
	// *** Get References
	SpringArm = PlayerActor->FindComponentByClass<USpringArmComponent>();
	defaultSpringArmLength = SpringArm->TargetArmLength;
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// DEBUG
	if (SpringArm && PlayerController) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Got all references"));
	}
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Missing a reference"));
	}
}


// Called every frame
void ULockOnTargeting::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	// *** Update Lock on Targeting
	if (bIsTargeting)
		UpdateTargeting();
	
	// *** Update Camera Reset
	else if (bIsCameraResetting)
		UpdateCameraReset(DeltaTime);
}


// Checks for nerest target, then either starts targeting or starts resetting camera
void ULockOnTargeting::OnTargetingInputStart() {

	// *** Get Nearest Actor With Targetable Tag
	TargetedActor = GetNearestTarget();

	// *** Start Lock On Targeting
	if (TargetedActor) {		// If there is a target to lock onto
		bIsTargeting = true;
	}

	// *** Start Camera Reset
	else {						// If there is no target to lock onto
		TargetRotation = PlayerActor->GetActorRotation();
		bIsCameraResetting = true;
	}
}


// Updates Spring arm offset to keep player and target centered and in view
void ULockOnTargeting::UpdateTargeting() {
	
	// *** Check to Stop Targeting
	if (!TargetedActor || SpringArm->TargetArmLength > MaxTargetingDistance) {
		
		// DEBUG
		if (GEngine)
		{
			float distance = FVector::Distance(TargetedActor->GetActorLocation(), PlayerActor->GetActorLocation());
			GEngine->AddOnScreenDebugMessage(
				-1, 5.0f, FColor::Yellow, 
				TEXT("Stopping Targeting, Distance between player and target = ") + 
				FString::SanitizeFloat(distance));
		}
		
		OnTargetingInputEnd();
		return;
	}

	// *** Update Spring Arm
	
	// Place spring arm base position in between player and target
	SpringArm->TargetOffset = (TargetedActor->GetActorLocation() - PlayerActor->GetActorLocation()) / 2;

	// Adjust the length of the spring arm so player and target stay in view
	SpringArm->TargetArmLength = 500 + SpringArm->TargetOffset.Length();


	// *** Update Camera
	TargetRotation = TargetingOffsetRotation;
	TargetRotation.Yaw += PlayerActor->GetActorRotation().Yaw;
	PlayerController->SetControlRotation(TargetRotation);	// Set rotation to player forward + offset
}


// Stops targeting and resets variables
void ULockOnTargeting::OnTargetingInputEnd() {

	bIsTargeting = false;
	TargetedActor = nullptr;
	SpringArm->TargetOffset = FVector::ZeroVector;
	SpringArm->TargetArmLength = defaultSpringArmLength;
}


// Returns the closest actor with targetable gameplay tag in sphere overlap
AActor* ULockOnTargeting::GetNearestTarget() {

	TArray<AActor*> nearbyActors;

	// *** Get All Actors in Sphere Overlap
	float sphereRadius = MaxTargetingDistance / 2.0f;
	FVector sphereStart = PlayerActor->GetActorLocation() +				// Make sphere infront of player
		(PlayerActor->GetActorForwardVector() * sphereRadius);
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), sphereStart,
		sphereRadius, ObjectTypes, nullptr, ActorsToIgnore, nearbyActors);

	// DEBUG
	#if WITH_EDITOR								// Only draw in editor, not in final build
		DrawDebugSphere(GetWorld(), sphereStart,
			sphereRadius, 15, FColor::Green, false, 1.0f, 0, 1.5f);
	#endif


	AActor* closestActor = nullptr;				// Closest actor
	float maxTargetingSqrDist = MaxTargetingDistance * MaxTargetingDistance;
	float closestDist = maxTargetingSqrDist;	// Distance to closest actor
	float curDist = FLT_MAX;					// Distance of current actor in loop

	// *** Find Closest Actor in Overlap With Targetable Tag
	for(AActor* actor : nearbyActors) {

		IGameplayTagAssetInterface* tagInterface = Cast<IGameplayTagAssetInterface>(actor);
		if (!tagInterface) continue;			// Check if actor implements the gameplay tags interface

		if (tagInterface->HasMatchingGameplayTag(TargetableTag)) {	// Check if actor has targetable tag

			curDist = actor->GetSquaredDistanceTo(PlayerActor);

			if (curDist < closestDist) {		// Found new closest actor
				closestDist = curDist;
				closestActor = actor;
			}
		}
	}

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
	}
}


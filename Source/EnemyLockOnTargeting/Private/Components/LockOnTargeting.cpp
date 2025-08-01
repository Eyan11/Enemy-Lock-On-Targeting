/*
* Author: Eyan Martucci
* Description:
*/

#include "Components/LockOnTargeting.h"

#include "Kismet/KismetSystemLibrary.h"			// For SphereOverlapActors
#include "GameplayTagAssetInterface.h"			// For IGameplayTagAssetInterface
#include "GameplayTagContainer.h"				// For FGameplayTag
#include "GameFramework/Actor.h"				// For AActor
#include "Engine/World.h"						// For GetWorld()
#include "Camera/CameraComponent.h"				// For UCameraComponent

// Sets default values for this component's properties
ULockOnTargeting::ULockOnTargeting()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULockOnTargeting::BeginPlay()
{
	Super::BeginPlay();

	// ...

	ActorsToIgnore.Add(GetOwner());	// Don't target self

	if (!Camera)
		Camera = GetOwner()->FindComponentByClass<UCameraComponent>();

	// DEBUG
	if (Camera) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Got camera reference"));
	}
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to get camera reference"));
	}
}


// Called every frame
void ULockOnTargeting::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// Starts targeting by switching to movement style and focuses camera in between target and player
void ULockOnTargeting::StartTargeting() {

	if (bIsTargeting) return;	// Ignore if already targeting

	bIsTargeting = true;
	targetedActor = GetNearestTarget();

	if (targetedActor) {
		// DEBUG
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Found an actor to target"));

	}
	else {
		// DEBUG
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Did not find actor to target"));

	}
}


// Stops targeting by switching to normal movement and focuses camera on player
void ULockOnTargeting::StopTargeting() {

	if (!bIsTargeting) return;	// Ignore if already not targeting

	bIsTargeting = false;
	targetedActor = nullptr;


}


// Returns the closest target in range
AActor* ULockOnTargeting::GetNearestTarget() {

	TArray<AActor*> nearbyActors;

	// Get all actors in radius
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetOwner()->GetActorLocation(), 
		TargetRadius,ObjectTypes, nullptr, ActorsToIgnore, nearbyActors);

	// DEBUG
	#if WITH_EDITOR						// Only draw in editor, not in final build
		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), 
			TargetRadius, 15, FColor::Green, false, 1.0f, 0, 1.5f);
	#endif


	float closestDist = FLT_MAX;		// Distance to closest actor
	float curDist = FLT_MAX;			// Distance of current actor in loop
	AActor* closestActor = nullptr;		// Closest actor

	// For all actors in radius
	for(AActor* actor : nearbyActors) {

		// Make sure actor has gameplay tags
		IGameplayTagAssetInterface* tagInterface = Cast<IGameplayTagAssetInterface>(actor);
		if (!tagInterface) continue;

		if (tagInterface->HasMatchingGameplayTag(TargetableTag)) {

			curDist = actor->GetSquaredDistanceTo(GetOwner());

			if (curDist < closestDist) {	// Found new closest actor
				closestDist = curDist;
				closestActor = actor;
			}
		}
	}

	return closestActor;
}


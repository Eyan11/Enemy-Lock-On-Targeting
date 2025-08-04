// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TargetingArrow.h"

#include "PaperSpriteComponent.h"		// For UPaperSpriteComponent
#include "Kismet/GameplayStatics.h"		// For GetPlayerCameraManager

// Sets default values
ATargetingArrow::ATargetingArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// *** Create Paper Sprite Component
	PaperSpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Paper Sprite"));
	RootComponent = PaperSpriteComp;

	// *** Get Camera Manager Reference
	CamManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
}

// Called when the game starts or when spawned
void ATargetingArrow::BeginPlay()
{
	Super::BeginPlay();
	
	HideArrow();
}

// Called every frame
void ATargetingArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// *** Update Arrow Location and Rotation
	if (PaperSpriteComp->IsVisible())
		UpdateArrow();
}


// Shows arrow above the targeted actor
void ATargetingArrow::SetTarget(AActor* NewTarget) {

	TargetActor = NewTarget;
	UpdateArrow();
	PaperSpriteComp->SetVisibility(true);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Showing Arrow"));
}

// Hides arrow and reset variables
void ATargetingArrow::HideArrow() {

	PaperSpriteComp->SetVisibility(false);
	TargetActor = nullptr;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Hiding Arrow"));
}

// Updates arrow location and rotation
void ATargetingArrow::UpdateArrow() {

	// *** Check if Target is Destroyed
	if (!TargetActor) {
		HideArrow();
		return;
	}

	// *** Set Location
	FVector targetLoc = TargetActor->GetActorLocation();
	targetLoc.Z += TargetActor->GetSimpleCollisionHalfHeight() + VerticalOffset;
	SetActorLocation(targetLoc);

	// *** Set Rotation
	
	FVector targetDir = CamManager->GetCameraLocation() - GetActorLocation();
	targetDir.Z = 0.0f;			// Prevent tilting up/down

	FRotator targetRot = FRotationMatrix::MakeFromXZ(targetDir, FVector::UpVector).Rotator();
	targetRot.Yaw += 90.0f;		// Rotate sprite to be perpendicular to camera

	SetActorRotation(targetRot);
}


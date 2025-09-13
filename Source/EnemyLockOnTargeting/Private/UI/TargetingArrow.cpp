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
	
	DynamicArrowMat = PaperSpriteComp->CreateDynamicMaterialInstance(0);
	HideArrow();
}

// Called every frame
void ATargetingArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// *** Update Arrow Location and Rotation
	if (PaperSpriteComp->IsVisible())
		UpdateArrow(DeltaTime);
}


// Shows arrow above the targeted actor
void ATargetingArrow::SetTarget(AActor* NewTarget) {

	TargetActor = NewTarget;
	UpdateArrow(0.0f);
	PaperSpriteComp->SetVisibility(true);
}

void ATargetingArrow::StartTargetingMode() {

	bIsTargetingMode = true;
	SetActorScale3D(FVector::OneVector);
	CurTime = 0.0f;				// Start at min height
	CurVerticalOffset = 0.0f;
	UpdateArrow(0.0f);
	DynamicArrowMat->SetVectorParameterValue("SpriteColor", TargetingArrowRedColor);
	PaperSpriteComp->SetVisibility(true);
}

void ATargetingArrow::StartNonTargetingMode() {

	bIsTargetingMode = false;
	SetActorScale3D(FVector::OneVector * 0.6);
	CurTime = (PI / 2.0f) / ArrowAlphaSpeed;	// Start with alpha = 1.0
	CurVerticalOffset = 0.0f;
	UpdateArrow(0.0f);
	DynamicArrowMat->SetVectorParameterValue("SpriteColor", NonTargetingArrowWhiteColor);
	PaperSpriteComp->SetVisibility(true);
}

// Hides arrow and reset variables
void ATargetingArrow::HideArrow() {

	PaperSpriteComp->SetVisibility(false);
	TargetActor = nullptr;
}

// Updates arrow location and rotation
void ATargetingArrow::UpdateArrow(float DeltaTime) {

	// *** Check if Target is Destroyed
	if (!TargetActor) {
		HideArrow();
		return;
	}

	// *** Loop Vertical Offset in Targeting Mode
	if (bIsTargetingMode) {
		CurTime += DeltaTime;
		CurVerticalOffset = VerticalBobCurve->GetFloatValue(CurTime * ArrowMoveSpeed) * TargetingVerticalMaxHeightOffset;
		// Applied when setting location below
	}
	// *** Loop Alpha in Non-Targeting Mode
	else {
		CurTime += DeltaTime;
		CurAlpha = (FMath::Sin(CurTime * ArrowAlphaSpeed) * 0.5f) + 0.5f;
		FLinearColor arrowColor(1.0f, 1.0f, 1.0f, CurAlpha);
		DynamicArrowMat->SetVectorParameterValue("SpriteColor", arrowColor);
	}


	// *** Set Location
	FVector targetLoc = TargetActor->GetActorLocation();
	targetLoc.Z += TargetActor->GetSimpleCollisionHalfHeight() + CurVerticalOffset + VerticalBaseHeight;
	SetActorLocation(targetLoc);

	// *** Set Rotation
	FVector targetDir = CamManager->GetCameraLocation() - GetActorLocation();
	targetDir.Z = 0.0f;			// Prevent tilting up/down

	FRotator targetRot = FRotationMatrix::MakeFromXZ(targetDir, FVector::UpVector).Rotator();
	targetRot.Yaw += 90.0f;		// Rotate sprite to be perpendicular to camera

	SetActorRotation(targetRot);
}


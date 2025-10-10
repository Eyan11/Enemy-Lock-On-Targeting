/*
* Author: Eyan Martucci
* Description: Manages the targeting arrow's transform and style
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetingArrow.generated.h"

UCLASS()
class ENEMYLOCKONTARGETING_API ATargetingArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetingArrow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


//*********************************************************


public:

	void SetTarget(AActor* NewTarget);
	void StartTargetingMode();
	void StartNonTargetingMode();
	void HideArrow();

private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")	// The component that displays the sprite
	class UPaperSpriteComponent* PaperSpriteComp;

	UPROPERTY(EditDefaultsOnly, Category = "References")
	UCurveFloat* VerticalBobCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Arrow")
	FLinearColor TargetingArrowRedColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Arrow")
	FLinearColor NonTargetingArrowWhiteColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Sprite")
	float VerticalBaseHeight = 70.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sprite")
	float TargetingVerticalMaxHeightOffset = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Arrow")
	float ArrowAlphaSpeed = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Arrow")
	float ArrowMoveSpeed = 1.0f;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicArrowMat;
	UPROPERTY()
	AActor* TargetActor;						// The actor that the arrow hovers above
	UPROPERTY()
	APlayerCameraManager* CamManager;
	UPROPERTY()
	float CurVerticalOffset = 0.0f;
	UPROPERTY()
	float CurAlpha = 1.0f;

	UPROPERTY()
	float CurTime;

	bool bIsTargetingMode = false;

	void UpdateArrow(float DeltaTime);			// Updates the arrow's location and rotation
};

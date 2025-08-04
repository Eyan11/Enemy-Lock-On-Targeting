// Fill out your copyright notice in the Description page of Project Settings.

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

	void SetTarget(AActor* NewTarget);					// Initializes the targeting arrow system
	void HideArrow();									// Cleans up the targeting arrow system

private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")	// The component that displays the sprite
		class UPaperSpriteComponent* PaperSpriteComp;

	UPROPERTY(EditDefaultsOnly, Category = "Sprite")	// The height above the target actor that the arrow hovers
		float VerticalOffset = 100.0f;

	AActor* TargetActor;								// The actor that the arrow hovers above
	APlayerCameraManager* CamManager;

	void UpdateArrow();									// Updates the arrow's location and rotation


};

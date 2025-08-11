// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerMeleeCombat.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENEMYLOCKONTARGETING_API UPlayerMeleeCombat : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerMeleeCombat();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


//*********************************************************


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")	// True if player is performing an attack
	bool bIsAttacking = false;

	void OnAttackInput();

private:

	UPROPERTY(EditDefaultsOnly, Category = "Animation")	// The animation montage played when doing a normal attack
	class UAnimMontage* NormalAttackMontage;

	class APlayerCharacter* PlayerCharacter = nullptr;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);	// Cleans up variables when normal attack finished (needs to be UFUNCTION)

};

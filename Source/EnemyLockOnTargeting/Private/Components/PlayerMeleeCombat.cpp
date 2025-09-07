/*
* Author: Eyan Martucci
* Description: Manages player attack and take damage
*/

#include "Components/PlayerMeleeCombat.h"

#include "Characters/PlayerCharacter.h"					// Player Character
#include "Components/CapsuleComponent.h"				// Capsule Component
#include "Kismet/GameplayStatics.h"						// Apply Damage
#include "GameFramework/CharacterMovementComponent.h"	// Character Movement (is falling)
#include "Animation/PlayerAnimInstance.h"				// Player Anim Instance
#include "Characters/EnemyCharacter.h"					// Enemy Character

// Sets default values for this component's properties
UPlayerMeleeCombat::UPlayerMeleeCombat()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPlayerMeleeCombat::BeginPlay()
{
	Super::BeginPlay();

	// *** Check Montage Reference
	if (!AttackMontage || !HurtMontage) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Missing reference to a montage in PlayerMeleeCombat"));
	}

	// *** Get Player Reference
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!PlayerCharacter) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("PlayerCharacter reference is null in PlayerMeleeCombat"));
		return;
	}

	// *** Get Player Anim Instance Reference
	PlayerAnimInstance = Cast<UPlayerAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance());
	if (!PlayerAnimInstance) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("PlayerAnimInstance reference is null in PlayerMeleeCombat"));
		return;
	}

	// *** Get Sword Collision Reference
	SwordCollision = PlayerCharacter->GetSwordCollision();
	if (!SwordCollision) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("SwordCollision is null in PlayerMeleeCombat"));
		return;
	}

	// *** Bind Sword Collision Overlap and Take Damage
	SwordCollision->OnComponentBeginOverlap.AddDynamic(this, &UPlayerMeleeCombat::OnSwordBeginOverlap);
	PlayerAnimInstance->OnMontageEnded.AddDynamic(this, &UPlayerMeleeCombat::OnMontageEnded);
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UPlayerMeleeCombat::OnTakeDamage);
}


// Called every frame
void UPlayerMeleeCombat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// Starts attack montage and hitbox
void UPlayerMeleeCombat::OnAttackInput() {

	if (!PlayerCharacter) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("PlayerCharacter is null in PlayerMeleeCombat.OnAttackInput()"));
		return;
	}

	if (!PlayerAnimInstance) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("PlayerAnimInstance is null in PlayerMeleeCombat"));
		return;
	}

	if (bIsAttacking || !bCanAttack) return;	// Ignore if already attacking or not allowed to


	// *** Play Normal Attack Montage
	PlayerAnimInstance->Montage_Play(AttackMontage);
	bIsAttacking = true;
	PlayerCharacter->StopMoveInput();
}


// Cleans up attack montage variables
void UPlayerMeleeCombat::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) {

	if (!PlayerCharacter) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("PlayerCharacter is null in PlayerMeleeCombat.OnAttackMontageEnded"));
		return;
	}

	if (Montage == AttackMontage) {			// Cleanup attack montage
		bIsAttacking = false;
		if (bInterrupted)
			DisableAttackCollision();
	}
	else if (Montage == HurtMontage)		// Cleanup hurt montage
		bCanAttack = true;

	if(!bInterrupted)						// Resume movement if no montage is playing after this one
		PlayerCharacter->ResumeMoveInput();
}


// Enables sword collision
void UPlayerMeleeCombat::EnableAttackCollision() {

	if (!SwordCollision) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("PlayerCharacter or SwordCollision is null in PlayerMeleeCombat"));
		return;
	}

	// *** Enable Sword Overlap Collision
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


// Disables sword collision
void UPlayerMeleeCombat::DisableAttackCollision() {

	if (!SwordCollision) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("PlayerCharacter or SwordCollision is null in PlayerMeleeCombat"));
		return;
	}

	// *** Disable Sword Collision
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


// Called when an actor begins to overlap with the sword collision component
void UPlayerMeleeCombat::OnSwordBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	// *** Damage Enemy
	if (PlayerCharacter && OtherActor && Cast<AEnemyCharacter>(OtherActor)) {

		UGameplayStatics::ApplyDamage(
			OtherActor,									// Actor to damage
			SwordDamage,								// Damage amount
			PlayerCharacter->GetInstigatorController(),	// Event instigator
			PlayerCharacter,							// Damage cause
			UDamageType::StaticClass()					// Damage type class
		);
	}
}


// Subscribed to Owner Actor's OnTakeAnyDamage event. 
// Player cannot die so it just starts take damage montage and doesn't track health.
void UPlayerMeleeCombat::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (!PlayerCharacter || Damage <= 0)
		return;

	float dotProd = 1.0f;	// Hurt player by default

	// *** Check if Shield Blocked Attack
	if (DamageCauser && PlayerCharacter->IsTargetingInputHeld()) {

		dotProd = FVector::DotProduct(PlayerCharacter->GetActorForwardVector(),
			DamageCauser->GetActorForwardVector());

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, FString::Printf(TEXT("Dot Product Result: %f"), dotProd));
	}

	// *** Block Attack
	if (dotProd < 0) {	// If attacker is facing player
		PlayerAnimInstance->Montage_Play(BlockMontage);
		bCanAttack = false;
		PlayerCharacter->StopMoveInput();

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("Player blocked attack"));
	}
	// *** Hurt Player
	else {				// If not shielding or attacker is facing same direction as player (behind)
		PlayerAnimInstance->Montage_Play(HurtMontage);
		bCanAttack = false;
		PlayerCharacter->StopMoveInput();

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("Player took damage"));
	}
}

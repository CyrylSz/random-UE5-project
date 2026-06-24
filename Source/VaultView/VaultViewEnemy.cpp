// Fill out your copyright notice in the Description page of Project Settings.
// ==============================================================================
// AI DISCLOSURE AND DOCUMENTATION:
// This file was implemented with the assistance of AI, based on the requirements 
// of the UEC project. The foundational concepts and initial structures were 
// adapted from the course conducted by Oleksandr Zimenko at PJATK, and have 
// been significantly expanded and modified using AI to meet the project's 
// specific technical requirements.
// ==============================================================================

#include "VaultViewEnemy.h"
#include "DamageableInterface.h"
#include "VaultViewAIController.h"
#include "Kismet/GameplayStatics.h"
#include "VaultViewCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AVaultViewEnemy::AVaultViewEnemy()
{
	// Tick must be enabled for movement and AI
	PrimaryActorTick.bCanEverTick = true;

	// Automatically possess AI when placed in world
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AVaultViewAIController::StaticClass();

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageCollision"));
	DamageCollision->SetupAttachment(RootComponent);
	DamageCollision->SetCollisionProfileName(TEXT("Trigger"));

	// Ensure hitbox doesn't affect NavMesh
	DamageCollision->SetCanEverAffectNavigation(false);
	
	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AVaultViewEnemy::OnDamageOverlapBegin);
	DamageCollision->OnComponentEndOverlap.AddDynamic(this, &AVaultViewEnemy::OnDamageOverlapEnd);
}

void AVaultViewEnemy::PlayAttackAnimation()
{
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage, 1.0f);
	}
}

void AVaultViewEnemy::OnDamageOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Dead enemy doesn't attack
	if (bIsDead) return;

	// Ensure the enemy only attacks the player
	if (OtherActor && OtherActor != this && OtherActor->IsA<AVaultViewCharacter>())
	{
		OverlappingPlayer = OtherActor;
		
		// Deal first damage immediately
		DealDamageTick();

		// Start looping damage timer
		GetWorldTimerManager().SetTimer(DamageTickTimer, this, &AVaultViewEnemy::DealDamageTick, 1.0f, true);
	}
}

void AVaultViewEnemy::OnDamageOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == OverlappingPlayer)
	{
		// Player escaped, stop damage timer
		GetWorldTimerManager().ClearTimer(DamageTickTimer);
		OverlappingPlayer = nullptr;
	}
}

void AVaultViewEnemy::DealDamageTick()
{
	// Dead enemy doesn't deal damage
	if (bIsDead || !OverlappingPlayer) return;

	// Play attack animation on each damage tick
	PlayAttackAnimation();

	// Call function via Interface
	IDamageableInterface* DamageableEntity = Cast<IDamageableInterface>(OverlappingPlayer);
	if (DamageableEntity)
	{
		DamageableEntity->TakeDamage(AttackDamage);
	}
}

void AVaultViewEnemy::TakeDamage(float DamageAmount)
{
	// Ignore damage if already dead
	if (bIsDead) return;

	HealthPoints -= DamageAmount;
	
	if (HealthPoints <= 0.0f)
	{
		Die();
	}
}

void AVaultViewEnemy::Die()
{
	// Prevent double death
	if (bIsDead) return;
	bIsDead = true;

	// 1. Stop damage timer and detach player
	GetWorldTimerManager().ClearTimer(DamageTickTimer);
	OverlappingPlayer = nullptr;
	DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 2. Detach AI controller (enemy stops chasing and attacking)
	DetachFromControllerPendingDestroy();

	// 3. Stop movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	// 3.5. Remove "Ambush" tag so the body disappears when the wave passes
	Tags.Remove(FName("Ambush"));

	// 4. Add kill to player (kill counter +1 in HUD)
	AVaultViewCharacter* Player = Cast<AVaultViewCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player)
	{
		Player->AddKill();
	}

	// 5. Randomly select death animation (death1 or death2)
	UAnimMontage* ChosenMontage = FMath::RandBool() ? DeathMontage1 : DeathMontage2;
	// Fallback: if one is null, use the other
	if (!ChosenMontage) ChosenMontage = DeathMontage1 ? DeathMontage1 : DeathMontage2;

	float MontageDuration = 0.1f;
	if (ChosenMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		float Played = GetMesh()->GetAnimInstance()->Montage_Play(ChosenMontage, 1.0f);
		if (Played > 0.f)
		{
			MontageDuration = Played;
		}
	}

	// 6. Enable ragdoll after animation finishes (with 0.05s buffer)
	GetWorldTimerManager().SetTimer(
		DeathAnimTimer,
		this,
		&AVaultViewEnemy::EnableRagdoll,
		FMath::Max(MontageDuration - 0.05f, 0.1f),
		false
	);
}

void AVaultViewEnemy::EnableRagdoll()
{
	if (!GetMesh()) return;

	// Enable physics simulation on mesh (ragdoll)
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	// Capsule no longer blocks - body lies freely
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

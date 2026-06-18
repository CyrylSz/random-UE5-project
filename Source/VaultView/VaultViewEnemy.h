// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimMontage.h"
#include "DamageableInterface.h"
#include "VaultViewEnemy.generated.h"

UCLASS()
class VAULTVIEW_API AVaultViewEnemy : public ACharacter, public IDamageableInterface
{
	GENERATED_BODY()

public:
	AVaultViewEnemy();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TObjectPtr<UBoxComponent> DamageCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 10.0f;

	// Attack animation montage assigned in BP_Assassin
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	// === DEATH ANIMATIONS ===
	// Assigned in BP_Assassin -> Class Defaults -> Animation
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeathMontage1;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeathMontage2;

	// Timer to enable ragdoll after death animation ends
	FTimerHandle DeathAnimTimer;

	// Plays attack animation via AnimInstance
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAttackAnimation();

	// Timer for cyclic damage dealing
	FTimerHandle DamageTickTimer;

	// Protection against Garbage Collector
	UPROPERTY(Transient)
	TObjectPtr<AActor> OverlappingPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float HealthPoints = 50.0f;

	// Flag - prevents multiple Die() calls
	bool bIsDead = false;

	// Enemy interface implementation
	virtual void TakeDamage(float DamageAmount) override;

	UFUNCTION()
	void OnDamageOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDamageOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DealDamageTick();

	// Main death logic: stops AI, plays animation, then enables ragdoll
	void Die();

	// Called after death animation - enables physics simulation (ragdoll)
	void EnableRagdoll();
};

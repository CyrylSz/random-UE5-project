// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
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

	// Timer do cyklicznego zadawania obrazen
	FTimerHandle DamageTickTimer;
	
	// Zabezpieczenie przed Garbage Collectorem (Wymóg 3.1)
	UPROPERTY(Transient)
	TObjectPtr<AActor> OverlappingPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float HealthPoints = 50.0f;

	// Wymóg 3.1 - Implementacja interfejsu przez wroga
	virtual void TakeDamage(float DamageAmount) override;

	UFUNCTION()
	void OnDamageOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDamageOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DealDamageTick();
};

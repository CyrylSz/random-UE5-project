// Fill out your copyright notice in the Description page of Project Settings.
// ==============================================================================
// AI DISCLOSURE AND DOCUMENTATION:
// This file was implemented entirely with the assistance of AI, to fulfill 
// the specific technical requirements of the UEC project.
// ==============================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VaultViewHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamagedSignature, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VAULTVIEW_API UVaultViewHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVaultViewHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float HealthPoints = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealthPoints = 100.0f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamagedSignature OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyHealthChange(float HealthChange);

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;
};

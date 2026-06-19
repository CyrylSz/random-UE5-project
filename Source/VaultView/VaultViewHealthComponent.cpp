// Fill out your copyright notice in the Description page of Project Settings.

#include "VaultViewHealthComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UVaultViewHealthComponent::UVaultViewHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	HealthPoints = 100.0f;
	MaxHealthPoints = 100.0f;
}


// Called when the game starts
void UVaultViewHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Broadcast initial health
	OnDamaged.Broadcast(HealthPoints, MaxHealthPoints);
}


// Called every frame
void UVaultViewHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UVaultViewHealthComponent::ApplyHealthChange(float HealthChange)
{
	if (IsDead()) return;

	HealthPoints = FMath::Clamp(HealthPoints + HealthChange, 0.0f, MaxHealthPoints);
	
	OnDamaged.Broadcast(HealthPoints, MaxHealthPoints);

	if (HealthPoints <= 0.0f)
	{
		OnDeath.Broadcast();
	}
}

bool UVaultViewHealthComponent::IsDead() const
{
	return HealthPoints <= 0.0f;
}

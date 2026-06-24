// Fill out your copyright notice in the Description page of Project Settings.
// ==============================================================================
// AI DISCLOSURE AND DOCUMENTATION:
// This file was implemented with the assistance of AI, based on the requirements 
// of the UEC project. The foundational concepts and initial structures were 
// adapted from the course conducted by Oleksandr Zimenko at PJATK, and have 
// been significantly expanded and modified using AI to meet the project's 
// specific technical requirements.
// ==============================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "PickupData.h" 
#include "PickupActor.generated.h"

UCLASS()
class VAULTVIEW_API APickupActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickupActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Logic")
	TSubclassOf<UPickupData> DataClass;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
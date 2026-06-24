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
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "VaultViewAIController.generated.h"

UCLASS()
class VAULTVIEW_API AVaultViewAIController : public AAIController
{
	GENERATED_BODY()

protected:
	AVaultViewAIController();

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UAIPerceptionComponent> PerceptionComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	// Explicitly defined components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBlackboardComponent> BlackboardComp;

	FTimerHandle LoseSightTimer;

	UPROPERTY(EditAnywhere)
	float LoseSightDelay = 3.0f;
};

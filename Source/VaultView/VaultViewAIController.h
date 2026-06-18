// Fill out your copyright notice in the Description page of Project Settings.

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

	// Wymóg 3.3 - Jawne zdefiniowane komponenty
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBlackboardComponent> BlackboardComp;

	FTimerHandle LoseSightTimer;

	UPROPERTY(EditAnywhere)
	float LoseSightDelay = 3.0f;
};

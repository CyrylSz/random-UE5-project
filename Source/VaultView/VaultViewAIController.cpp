// Fill out your copyright notice in the Description page of Project Settings.
// ==============================================================================
// AI DISCLOSURE AND DOCUMENTATION:
// This file was implemented with the assistance of AI, based on the requirements 
// of the UEC project. The foundational concepts and initial structures were 
// adapted from the course conducted by Oleksandr Zimenko at PJATK, and have 
// been significantly expanded and modified using AI to meet the project's 
// specific technical requirements.
// ==============================================================================

#include "VaultViewAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "VaultViewCharacter.h" // Check if we see the player
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "VaultViewCharacter.h" // Check if we see the player

AVaultViewAIController::AVaultViewAIController()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*PerceptionComp);

	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SenseConfig"));
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Remove filters so AI can see the player
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	PerceptionComp->ConfigureSense(*SightConfig);

	UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1500.0f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	PerceptionComp->ConfigureSense(*HearingConfig);

	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

	// Initialize explicit AI components
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void AVaultViewAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor->IsA<AVaultViewCharacter>()) return;

	// Use our explicitly defined Blackboard
	if (!BlackboardComp) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
		BlackboardComp->SetValueAsBool(TEXT("HasLineOfSight"), true);
		BlackboardComp->SetValueAsVector(TEXT("LastKnownLocation"), Actor->GetActorLocation());
		
		GetWorldTimerManager().ClearTimer(LoseSightTimer);
	}
	else
	{
		GetWorldTimerManager().SetTimer(LoseSightTimer, [this]()
		{
			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsBool(TEXT("HasLineOfSight"), false);
			}
		}, LoseSightDelay, false);
	}
}

void AVaultViewAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTree && BehaviorTree->BlackboardAsset)
	{
		// Connect our components to Unreal Engine pointers
		Blackboard = BlackboardComp;
		BrainComponent = BehaviorTreeComponent;

		BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
	}

	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AVaultViewAIController::OnPerceptionUpdated);
}

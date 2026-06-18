// Fill out your copyright notice in the Description page of Project Settings.

#include "VaultViewAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "VaultViewCharacter.h" // Sprawdzamy, czy widzimy gracza
#include "Perception/AISenseConfig_Sight.h"
#include "VaultViewCharacter.h" // Sprawdzamy, czy widzimy gracza

AVaultViewAIController::AVaultViewAIController()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*PerceptionComp);

	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SenseConfig"));
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// POPRAWKA 3: Zdejmujemy filtry, żeby AI na 100% zobaczyło gracza
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	PerceptionComp->ConfigureSense(*SightConfig);

	// Inicjalizacja jawnych komponentów AI
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void AVaultViewAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor->IsA<AVaultViewCharacter>()) return;

	// Używamy naszego, jawnie zdefiniowanego Blackboarda
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
		// POPRAWKA KRYTYCZNA: Podpięcie naszych komponentów pod wskaźniki silnika Unreal Engine
		Blackboard = BlackboardComp;
		BrainComponent = BehaviorTreeComponent;

		BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
	}

	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AVaultViewAIController::OnPerceptionUpdated);
}

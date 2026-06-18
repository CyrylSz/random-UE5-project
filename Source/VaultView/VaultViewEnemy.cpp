// Fill out your copyright notice in the Description page of Project Settings.

#include "VaultViewEnemy.h"
#include "DamageableInterface.h"
#include "VaultViewAIController.h"
#include "Kismet/GameplayStatics.h"
#include "VaultViewCharacter.h"

AVaultViewEnemy::AVaultViewEnemy()
{
	// POPRAWKA 1: Tick MUSI być włączony, inaczej ruch postaci i AI zamarza!
	PrimaryActorTick.bCanEverTick = true;

	// Automatyczne przypisanie kontrolera Artificial Intelligence po umieszczeniu na mapie
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AVaultViewAIController::StaticClass();

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageCollision"));
	DamageCollision->SetupAttachment(RootComponent);
	DamageCollision->SetCollisionProfileName(TEXT("Trigger"));

	// POPRAWKA 2 (Twoja): Gwarancja w C++, że hitbox nie wytnie dziury w NavMeshu!
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
	// POPRAWKA: Upewniamy się, że wróg atakuje TYLKO gracza, a nie samego siebie lub kolegów!
	if (OtherActor && OtherActor != this && OtherActor->IsA<AVaultViewCharacter>())
	{
		OverlappingPlayer = OtherActor;
		
		// Zadajemy pierwsze obrazenia natychmiast
		DealDamageTick();

		// Uruchamiamy timer, by powtarzal funkcje co 1 sekunde w zapetleniu
		GetWorldTimerManager().SetTimer(DamageTickTimer, this, &AVaultViewEnemy::DealDamageTick, 1.0f, true);
	}
}

void AVaultViewEnemy::OnDamageOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == OverlappingPlayer)
	{
		// Gracz uciekl, zatrzymujemy timer obrazen
		GetWorldTimerManager().ClearTimer(DamageTickTimer);
		OverlappingPlayer = nullptr;
	}
}

void AVaultViewEnemy::DealDamageTick()
{
	if (OverlappingPlayer)
	{
		// Odtwarzamy animacje ataku przy kazdym tick'u obrazen
		PlayAttackAnimation();

		// Wywolujemy funkcje poprzez Interfejs!
		IDamageableInterface* DamageableEntity = Cast<IDamageableInterface>(OverlappingPlayer);
		if (DamageableEntity)
		{
			DamageableEntity->TakeDamage(AttackDamage);
		}
	}
}

void AVaultViewEnemy::TakeDamage(float DamageAmount)
{
	HealthPoints -= DamageAmount;
	
	if (HealthPoints <= 0.0f)
	{
		// POPRAWKA: Odnalezienie gracza i dodanie punktu zabójstwa przed śmiercią
		AVaultViewCharacter* Player = Cast<AVaultViewCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			Player->AddKill();
		}

		Destroy();
	}
}

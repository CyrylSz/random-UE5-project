// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupActor.h"
#include "VaultViewCharacter.h" 

// Sets default values
APickupActor::APickupActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(CollisionComponent);
	
	StaticMesh->SetupAttachment(CollisionComponent);
	
	CollisionComponent->SetCollisionProfileName(TEXT("Trigger"));
	StaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::OnOverlapBegin);
}

void APickupActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AVaultViewCharacter* PlayerCharacter = Cast<AVaultViewCharacter>(OtherActor);
	
	if (PlayerCharacter && DataClass)
	{
		UPickupData* ItemData = NewObject<UPickupData>(this, DataClass);
		
		PlayerCharacter->Score += ItemData->ScoreChange;
		PlayerCharacter->HealthPoints += ItemData->HealthPointsChange;
		
		if (PlayerCharacter->HealthPoints > PlayerCharacter->MaxHealthPoints)
		{
			PlayerCharacter->HealthPoints = PlayerCharacter->MaxHealthPoints;
		}

		PlayerCharacter->Inventory.Add(ItemData);

		this->Destroy();
	}
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// Copyright Epic Games, Inc. All Rights Reserved.
// ==============================================================================
// AI DISCLOSURE AND DOCUMENTATION:
// This file was implemented with the assistance of AI, based on the requirements 
// of the UEC project. The foundational concepts and initial structures were 
// adapted from the course conducted by Oleksandr Zimenko at PJATK, and have 
// been significantly expanded and modified using AI to meet the project's 
// specific technical requirements.
// ==============================================================================

#include "VaultViewCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerStart.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "VaultView.h"
#include "VaultViewHUDWidget.h"
#include "VaultViewEnemy.h"
#include "VaultViewHealthComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AVaultViewCharacter::AVaultViewCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	// Attach the camera to the head of the character model
	FirstPersonCameraComponent->SetupAttachment(GetMesh(), FName("head"));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	// By default, the First-Person camera is disabled (we start in Third-Person Perspective)
	FirstPersonCameraComponent->SetActive(false); 

	// Create weapon component - hidden until picked up
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), FName("WeaponSocket")); // Socket in character's hand
	WeaponMesh->SetVisibility(false);                               // Hidden by default
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);// Weapon does not block

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	HealthComponent = CreateDefaultSubobject<UVaultViewHealthComponent>(TEXT("HealthComponent"));
}

void AVaultViewCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Set initial visibility based on the wave
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		UpdateWaveVisibility(WaveCount);
	}
}

void AVaultViewCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind death event independently of HUD
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AVaultViewCharacter::ShowDeathScreen);
	}

	// Create HUD widget from editor class
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UVaultViewHUDWidget>(GetWorld(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport(); // Show on screen

			// Bind widget functions to character delegates
			if (HealthComponent)
			{
				HealthComponent->OnDamaged.AddDynamic(HUDWidget, &UVaultViewHUDWidget::UpdateHP);
			}
			OnWaveChanged.AddDynamic(HUDWidget, &UVaultViewHUDWidget::UpdateWave);
			OnKillsChanged.AddDynamic(HUDWidget, &UVaultViewHUDWidget::UpdateKills);
			OnScoreChanged.AddDynamic(HUDWidget, &UVaultViewHUDWidget::UpdateScore);

			// Refresh UI on game start
			if (HealthComponent)
			{
				HealthComponent->OnDamaged.Broadcast(HealthComponent->HealthPoints, HealthComponent->MaxHealthPoints);
			}
			OnWaveChanged.Broadcast(WaveCount);
			OnKillsChanged.Broadcast(KillCount);
			OnScoreChanged.Broadcast(Score);
		}
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}
}

void AVaultViewCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVaultViewCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AVaultViewCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVaultViewCharacter::Look);

		// Attack (requires IA_Attack assignment in BP_ThirdPersonCharacter)
		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AVaultViewCharacter::TryAttack);
		}
	}
	else
	{
		UE_LOG(LogVaultView, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AVaultViewCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AVaultViewCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AVaultViewCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AVaultViewCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AVaultViewCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AVaultViewCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AVaultViewCharacter::ToggleCameraPerspective()
{
	if (FollowCamera->IsActive())
	{
		// Switch to First-Person Perspective
		FollowCamera->SetActive(false);
		FirstPersonCameraComponent->SetActive(true);
		bUseControllerRotationYaw = true; // Rotate the body along with the camera
	}
	else
	{
		// Return to Third-Person Perspective
		FirstPersonCameraComponent->SetActive(false);
		FollowCamera->SetActive(true);
		bUseControllerRotationYaw = false;
	}
}

void AVaultViewCharacter::ApplyHealthChange(float HealthChange)
{
	if (HealthComponent)
	{
		HealthComponent->ApplyHealthChange(HealthChange);
	}
}

void AVaultViewCharacter::ShowDeathScreen()
{
	if (DeathWidgetClass)
	{
		UUserWidget* DeathWidget = CreateWidget<UUserWidget>(GetWorld(), DeathWidgetClass);
		if (DeathWidget)
		{
			DeathWidget->AddToViewport();
			
			// Pause game and show mouse cursor
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				PC->bShowMouseCursor = true;
				PC->SetInputMode(FInputModeUIOnly());
				UGameplayStatics::SetGamePaused(GetWorld(), true);
			}
		}
	}
}

void AVaultViewCharacter::AddScore(int32 ScoreToAdd)
{
	Score += ScoreToAdd;
	OnScoreChanged.Broadcast(Score);
}

void AVaultViewCharacter::AddKill()
{
	KillCount++;
	OnKillsChanged.Broadcast(KillCount);
}

void AVaultViewCharacter::NextWave()
{
	WaveCount++;
	OnWaveChanged.Broadcast(WaveCount);
}

void AVaultViewCharacter::TakeDamage(float DamageAmount)
{
	// Subtract health points
	ApplyHealthChange(-DamageAmount);
}

void AVaultViewCharacter::EquipAxe()
{
	bHasWeapon = true;
	if (WeaponMesh)
	{
		WeaponMesh->SetVisibility(true);
	}
}

void AVaultViewCharacter::TryAttack()
{
	// Attack possible only with a weapon
	if (!bHasWeapon) return;

	// Sphere trace in front of player (range 150cm, radius 50cm)
	const FVector Start = GetActorLocation();
	const FVector End   = Start + GetActorForwardVector() * 150.0f;

	TArray<FHitResult> Hits;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(50.0f);

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn); // Detects other Pawns (enemies)

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Ignore the player

	GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjParams, Sphere, QueryParams);

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == this) continue;

		// Call TakeDamage via interface (works on any IDamageable, including Assassin)
		IDamageableInterface* Damageable = Cast<IDamageableInterface>(HitActor);
		if (Damageable)
		{
			Damageable->TakeDamage(AttackDamage);
		}
	}
}

// ============================================================
// WAVE SYSTEM
// ============================================================

void AVaultViewCharacter::TriggerWaveEnd()
{
	// Prevent multiple triggers if transition is already in progress
	if (bWaveTransitioning) return;
	bWaveTransitioning = true;

	// Block player movement during transition
	GetCharacterMovement()->DisableMovement();

	// Start screen fade: transition from 0 (transparent) to 1 (black) over 0.5s
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(
			0.0f, 1.0f,  // from transparent to black
			0.5f,         // over 0.5 seconds
			FLinearColor::Black,
			false,        // do not fade audio
			true          // hold on black after completion
		);
	}

	// After 0.6s (0.1s buffer after full black), perform reset
	GetWorldTimerManager().SetTimer(
		WaveTransitionTimer,
		this,
		&AVaultViewCharacter::CompleteWaveTransition,
		0.6f,
		false
	);
}

void AVaultViewCharacter::CompleteWaveTransition()
{
	// 1. Teleport player to PlayerStart
	AActor* StartActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	if (StartActor)
	{
		// +96 = half character capsule height, to prevent spawning under the floor
		FVector SpawnLocation = StartActor->GetActorLocation() + FVector(0.f, 0.f, 96.f);
		TeleportTo(SpawnLocation, StartActor->GetActorRotation());

		// Reset controller (camera) rotation to PlayerStart direction
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PC->SetControlRotation(StartActor->GetActorRotation());
		}
	}

	// 2. Increase wave counter (triggers OnWaveChanged delegate -> HUD update)
	NextWave();

	// 3. Show message in the center of the screen (text for the JUST PASSED wave)
	if (HUDWidget)
	{
		HUDWidget->ShowWaveMessage(GetWaveMessage(WaveCount - 1));
	}

	// 4. Destroy old enemies and spawn new ones for the current wave
	SpawnEnemiesForWave(WaveCount);

	// 5. Update visibility of objects related to specific wave
	UpdateWaveVisibility(WaveCount);

	// 6. Restore player movement
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// 7. Brighten screen: transition from black to transparent over 0.5s
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(
			1.0f, 0.0f,  // from black to transparent
			0.5f,
			FLinearColor::Black,
			false,
			false         // do not hold - return to normal image
		);
	}

	bWaveTransitioning = false;
}

FString AVaultViewCharacter::GetWaveMessage(int32 WaveIndex) const
{
	// Message displayed when transitioning FROM given wave to the next
	switch (WaveIndex)
	{
		case 0: return TEXT("YOU WILL NEVER ESCAPE");
		case 1: return TEXT("IMPRESSIVE... BUT USELESS");
		case 2: return TEXT("OK... NOW IT'S REALLY OVER FOR YOU!");
		default: return TEXT("...");
	}
}

void AVaultViewCharacter::SpawnEnemiesForWave(int32 Wave)
{
	// Destroy all living enemies from previous wave (except those with "Ambush" tag)
	TArray<AActor*> ExistingEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVaultViewEnemy::StaticClass(), ExistingEnemies);
	for (AActor* Enemy : ExistingEnemies)
	{
		// Skip pre-spawned enemies from traps / ceiling, so they are not removed from the map
		if (Enemy && !Enemy->ActorHasTag(TEXT("Ambush")))
		{
			Enemy->Destroy();
		}
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (Wave == 1)
	{
		// Wave 1: Assassins spawn via BP_AmbushTrigger in the corridor
		// (not here - trigger volume will handle it when player enters the zone)
		return;
	}

	if (Wave == 2 && EliteAssassinClass)
	{
		// Wave 2: 1 Elite Assassin - search for points with "SpawnElite" tag
		TArray<AActor*> EliteSpawnPoints;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SpawnElite"), EliteSpawnPoints);

		if (EliteSpawnPoints.Num() > 0)
		{
			GetWorld()->SpawnActor<ACharacter>(
				EliteAssassinClass,
				EliteSpawnPoints[0]->GetActorTransform(),
				SpawnParams
			);
		}
	}
	// Wave 0 and 3+: no spawn
}

void AVaultViewCharacter::UpdateWaveVisibility(int32 CurrentWave)
{
	// Create current wave prefix, e.g. "Wave_1"
	FString CurrentWaveTag = FString::Printf(TEXT("Wave_%d"), CurrentWave);
	
	// Get all actors from scene (to check their tags)
	// Only take actors that can be hidden/shown (not UI, PlayerController, etc.)
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
	
	for (AActor* Actor : AllActors)
	{
		if (!Actor) continue;
		
		bool bHasAnyWaveTag = false;
		bool bMatchesCurrentWave = false;
		
		// Search actor tags
		for (const FName& Tag : Actor->Tags)
		{
			FString TagStr = Tag.ToString();
			
			// If tag starts with "Wave_" (e.g. Wave_0, Wave_1, Wave_99)
			if (TagStr.StartsWith(TEXT("Wave_")))
			{
				bHasAnyWaveTag = true;
				
				// If it is a tag EXACTLY matching current wave
				if (TagStr == CurrentWaveTag)
				{
					bMatchesCurrentWave = true;
					break; // Found its tag, no need to search further
				}
			}
		}
		
		// If actor has ANY wave tag (meaning it was included in our visibility system)
		if (bHasAnyWaveTag)
		{
			// Hide/show entire actor and its components (true = hidden, false = visible)
			Actor->SetActorHiddenInGame(!bMatchesCurrentWave);
			Actor->SetActorEnableCollision(bMatchesCurrentWave); // Also disable collision
		}
	}
}



// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "VaultViewCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamagedSignature, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatsChangedSignature, int32, NewValue);

UCLASS(abstract)
class AVaultViewCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
public:
	// Camera for First-Person Perspective
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	// Required project variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float HealthPoints = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float MaxHealthPoints = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float AttackDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Score = 0;

	// Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<TObjectPtr<class UPickupData>> Inventory;

	// Function to change Point of View
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ToggleCameraPerspective();

	// Project required delegates
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamagedSignature OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatsChangedSignature OnWaveChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatsChangedSignature OnKillsChanged;

	// New project statistics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 KillCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 WaveCount = 1;

	// Score delegate
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatsChangedSignature OnScoreChanged;

	// Widget references (Type safety)
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UVaultViewHUDWidget> HUDWidgetClass;

	// Functions modifying stats during gameplay (Trigger delegates)
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void AddScore(int32 ScoreToAdd);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void AddKill();

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void NextWave();

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> DeathWidgetClass;

	// Pointer to created HUD widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<class UVaultViewHUDWidget> HUDWidget;

	// Safe function for applying damage and healing
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void ApplyHealthChange(float HealthChange);

	void ShowDeathScreen();

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

public:

	/** Constructor */
	AVaultViewCharacter();	

protected:
	virtual void BeginPlay() override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


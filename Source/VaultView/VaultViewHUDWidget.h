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
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "VaultViewHUDWidget.generated.h"

UCLASS()
class VAULTVIEW_API UVaultViewHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// meta=(BindWidget) forces exact naming of elements in the editor
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WaveText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> KillText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	// Central text displayed during wave transition (BindWidgetOptional = does not crash if missing in BP)
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> WaveMessageText;

	// Interface update functions
	UFUNCTION()
	void UpdateHP(float CurrentHP, float MaxHP);

	UFUNCTION()
	void UpdateWave(int32 CurrentWave);

	UFUNCTION()
	void UpdateKills(int32 CurrentKills);

	UFUNCTION()
	void UpdateScore(int32 CurrentScore);

	// Displays large text in the center of the screen for 3 seconds (e.g. "YOU WILL NEVER ESCAPE")
	UFUNCTION()
	void ShowWaveMessage(FString Message);

private:
	FTimerHandle MessageHideTimer;

	void HideWaveMessage();
};


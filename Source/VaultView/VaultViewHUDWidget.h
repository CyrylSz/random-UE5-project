// Fill out your copyright notice in the Description page of Project Settings.

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

	// Interface update functions
	UFUNCTION()
	void UpdateHP(float CurrentHP, float MaxHP);

	UFUNCTION()
	void UpdateWave(int32 CurrentWave);

	UFUNCTION()
	void UpdateKills(int32 CurrentKills);

	UFUNCTION()
	void UpdateScore(int32 CurrentScore);
};

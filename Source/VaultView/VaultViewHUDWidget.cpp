// Fill out your copyright notice in the Description page of Project Settings.

#include "VaultViewHUDWidget.h"

void UVaultViewHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	if (HPBar && MaxHP > 0.0f)
	{
		HPBar->SetPercent(CurrentHP / MaxHP);
	}
}

void UVaultViewHUDWidget::UpdateWave(int32 CurrentWave)
{
	if (WaveText)
	{
		WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave)));
	}
}

void UVaultViewHUDWidget::UpdateKills(int32 CurrentKills)
{
	if (KillText)
	{
		KillText->SetText(FText::FromString(FString::Printf(TEXT("Kills: %d"), CurrentKills)));
	}
}

void UVaultViewHUDWidget::UpdateScore(int32 CurrentScore)
{
	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), CurrentScore)));
	}
}

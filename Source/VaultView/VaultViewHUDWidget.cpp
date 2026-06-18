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

void UVaultViewHUDWidget::ShowWaveMessage(FString Message)
{
	if (!WaveMessageText) return;

	// Set text and show
	WaveMessageText->SetText(FText::FromString(Message));
	WaveMessageText->SetVisibility(ESlateVisibility::Visible);

	// Hide automatically after 3 seconds
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			MessageHideTimer,
			this,
			&UVaultViewHUDWidget::HideWaveMessage,
			3.0f,
			false
		);
	}
}

void UVaultViewHUDWidget::HideWaveMessage()
{
	if (WaveMessageText)
	{
		WaveMessageText->SetVisibility(ESlateVisibility::Hidden);
	}
}


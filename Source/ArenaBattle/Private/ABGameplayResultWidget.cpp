// Fill out your copyright notice in the Description page of Project Settings.

#include "ABGameplayResultWidget.h"
#include "Components/TextBlock.h"
#include "ABGameState.h"

void UABGameplayResultWidget::BindGameState(AABGameState* GameState) {
	ABCHECK(GameState != nullptr);
	CurrentGameState = GameState;
}

void UABGameplayResultWidget::NativeConstruct() {
	Super::NativeConstruct();

	ABCHECK(CurrentGameState.IsValid())

	auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
	auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
	ABCHECK(Result != nullptr && TotalScore != nullptr);

	Result->SetText(FText::FromString(CurrentGameState->IsGameCleared() ? TEXT("Mission Complete!") : TEXT("Mission FUCK")));
	TotalScore->SetText(FText::FromString(FString::FromInt(CurrentGameState->GetTotalGameScore())));
}




// Fill out your copyright notice in the Description page of Project Settings.

#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameState.h"

AABGameMode::AABGameMode()
{
	DefaultPawnClass = AABCharacter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass();
	GameStateClass = AABGameState::StaticClass();
	ScoreToClear = 2;
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
	if (ABPlayerState != nullptr) {
		ABPlayerState->InitPlayerData();
	}
}

void AABGameMode::PostInitializeComponents() {
	Super::PostInitializeComponents();
	ABGameState = Cast<AABGameState>(GameState);
}

void AABGameMode::AddScore(AABPlayerController* ScoredPlayer) {
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
		if (ABPlayerController != nullptr && ScoredPlayer == ABPlayerController) {
			// 사람이 조작하는 pawn 이며 자기 자신의 경우
			ABPlayerController->AddGameScore();
			break;
		}
	}

	ABGameState->AddGameScore();

	if (GetScore() >= ScoreToClear) {
		ABGameState->SetGameCleared();

		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It) {
			(*It)->TurnOff();
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
			const auto ABPlayerController = Cast<AABPlayerController>(It->Get());

			if (nullptr != ABPlayerController) {
				ABPlayerController->ShowResultUI();
			}
		}
	}
}

int32 AABGameMode::GetScore() const {
	return ABGameState->GetTotalGameScore();
}
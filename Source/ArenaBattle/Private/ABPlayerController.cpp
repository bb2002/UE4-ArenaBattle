// Fill out your copyright notice in the Description page of Project Settings.

#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"
#include "ABGameplayWidget.h"
#include "ABGameplayResultWidget.h"
#include "ABGameState.h"

AABPlayerController::AABPlayerController() {
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UIHUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
	if (UIHUD_C.Succeeded()) {
		HUDWidgetClass = UIHUD_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UABGameplayWidget> UIMenu_C(TEXT("/Game/Book/UI/UI_Menu.UI_Menu_C"));
	if (UIMenu_C.Succeeded()) {
		MenuWidgetClass = UIMenu_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UABGameplayResultWidget> UIResult_C(TEXT("/Game/Book/UI/UI_Result.UI_Result_C"));
	if (UIResult_C.Succeeded()) {
		ResultWidgetClass = UIResult_C.Class;
	}
}

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPlayerController::Possess(APawn *aPawn)
{
	ABLOG_S(Warning);
	Super::Possess(aPawn);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ChangeInputMode(true);

	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1);

	ResultWidget = CreateWidget<UABGameplayResultWidget>(this, ResultWidgetClass);
	ABCHECK(nullptr != ResultWidget);
	
	ABPlayerState = Cast<AABPlayerState>(PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	HUDWidget->BindPlayerState(ABPlayerState);
	ABPlayerState->OnPlayerStateChange.Broadcast();
}

UABHUDWidget* AABPlayerController::GetHUDWidget() const {
	return HUDWidget;
}

void AABPlayerController::NPCKill(AABCharacter* KilledNPC) const {
	ABPlayerState->AddExp(KilledNPC->GetExp());
}

void AABPlayerController::AddGameScore() const {
	ABPlayerState->AddGameScore();
}

void AABPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("GamePause"), EInputEvent::IE_Pressed, this, &AABPlayerController::OnGamePause);
}

void AABPlayerController::OnGamePause() {
	MenuWidget = CreateWidget<UABGameplayWidget>(this, MenuWidgetClass);
	ABCHECK(nullptr != MenuWidget);
	MenuWidget->AddToViewport(3);

	SetPause(true);
	ChangeInputMode(false);
}

void AABPlayerController::ChangeInputMode(bool bGameMode) {
	if (bGameMode) {
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else {
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::ShowResultUI() {
	auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
	ABCHECK(ABGameState != nullptr);
	ResultWidget->BindGameState(ABGameState);

	ResultWidget->AddToViewport();
	ChangeInputMode(false);
}
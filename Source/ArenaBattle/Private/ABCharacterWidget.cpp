// Fill out your copyright notice in the Description page of Project Settings.

#include "ABCharacterWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewComponent) {
	ABCHECK(NewComponent != nullptr);

	CurrentCharacterStat = NewComponent;
	NewComponent->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
}

void UABCharacterWidget::NativeConstruct() {
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPProgress")));
	if (HPProgressBar != nullptr) {
		UpdateHPWidget();
	}
}

void UABCharacterWidget::UpdateHPWidget() {
	if (CurrentCharacterStat.IsValid() && nullptr != HPProgressBar) {
		HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
	}
}





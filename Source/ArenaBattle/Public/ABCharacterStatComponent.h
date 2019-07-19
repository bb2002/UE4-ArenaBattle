// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnIsZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnChangedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:	
	void SetNewLevel(int32 Level);
	void SetDamage(float NewDamage);
	float GetAttack();
	void SetHP(float NewHP);
	float GetHPRatio();
	int32 GetDropExp() const;

	FOnIsZeroDelegate OnHPIsZero;
	FOnChangedDelegate OnHPChanged;

private:
	struct FABCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "State", Meta = (AllowPrivateAccess = true))
		int32 Level;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "State", Meta = (AllowPrivateAccess = true))
		float CurrentHP;
	
};

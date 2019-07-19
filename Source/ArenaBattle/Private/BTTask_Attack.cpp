// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_Attack.h"
#include "ABAIController.h"
#include "ABCharacter.h"

UBTTask_Attack::UBTTask_Attack() {
	NodeName = TEXT("Attack");
	bNotifyTick = true;
	IsAttacking = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerCompo, uint8* NodeMemory) {
	Super::ExecuteTask(OwnerCompo, NodeMemory);

	auto ABCharacter = Cast<AABCharacter>(OwnerCompo.GetAIOwner()->GetPawn());
	if (ABCharacter == nullptr) {
		return EBTNodeResult::Failed;
	}

	ABCharacter->Attack();
	IsAttacking = true;
	ABCharacter->OnAttackEnd.AddLambda([this]() -> void {
		IsAttacking = false;
	});

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerCompo, uint8* NodeMemory, float DeltaSec) {
	Super::TickTask(OwnerCompo, NodeMemory, DeltaSec);
	if (!IsAttacking) {
		FinishLatentTask(OwnerCompo, EBTNodeResult::Succeeded);
	}
}



// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_TurnToTarget.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget() {
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& Owner, uint8* NodeMemory) {
	Super::ExecuteTask(Owner, NodeMemory);

	auto ABCharacter = Cast<AABCharacter>(Owner.GetAIOwner()->GetPawn());
	if (nullptr == ABCharacter) return EBTNodeResult::Failed;

	auto Target = Cast<AABCharacter>(Owner.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
	if (Target == nullptr) return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - ABCharacter->GetActorLocation();
	LookVector.Z = 0.0F;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	ABCharacter->SetActorRotation(FMath::RInterpTo(ABCharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0F));

	return EBTNodeResult::Succeeded;
}



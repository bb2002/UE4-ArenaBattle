// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABSection.generated.h"

UCLASS()
class ARENABATTLE_API AABSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABSection();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	enum class ESectionState : uint8
	{
		READY = 0,
		BATTLE,
		COMPLETE
	};

	void SetState(ESectionState NewState);
	ESectionState CurrentState = ESectionState::READY;

	void OperateGates(bool bOpen = true);

	UPROPERTY(VisibleAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = true))
	TArray<UStaticMeshComponent*> GateMeshes;
	
	UPROPERTY(VisibleAnywhere, Category = "Trigger", Meta = (AllowPrivateAccess = true))
		TArray<UBoxComponent*> GateTriggers;

	UPROPERTY(VisibleAnywhere, Category = "Trigger", Meta = (AllowPrivateAccess = true))
		UBoxComponent* Trigger;

	UPROPERTY(EditAnywhere, Category = "State", Meta = (AllowPrivateAccess = true))
		bool bNoBattle;

	UPROPERTY(EditAnywhere, Category = "State", Meta = (AllowPrivateAccess = true))
		int32 SpawnNPCSize;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedCompo, AActor* OtherActor, UPrimitiveComponent* OtherCompo, int32 OtherBodyIdx, bool bFromSwwep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedCompo, AActor* OtherActor, UPrimitiveComponent* OtherCompo, int32 OtherBodyIdx, bool bFromSwwep, const FHitResult& SweepResult);

	void OnNPCSpawn();

	UFUNCTION()
		void OnKeyNPCDestoryed(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere, Category = "Spawn", Meta = (AllowPrivateAccess = true))
		float EnemySpawnTime;

	UPROPERTY(EditAnywhere, Category = "Spawn", Meta = (AllowPrivateAccess = true))
		float ItemBoxSpawnTime;

	FTimerHandle SpawnNPCTimerHandle = {};
	FTimerHandle SpawnItemBoxTimerHandle = {};
};

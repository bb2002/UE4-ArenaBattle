// Fill out your copyright notice in the Description page of Project Settings.

#include "ABSection.h"
#include "ABCharacter.h"
#include "ABItemBox.h"
#include "ABPlayerController.h"
#include "ABGameMode.h"


// Sets default values
AABSection::AABSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	UStaticMeshComponent* Mesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh2;

	FString AssetPath = TEXT("StaticMesh'/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE'");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SMSquare(*AssetPath);

	if (SMSquare.Succeeded()) {
		Mesh->SetStaticMesh(SMSquare.Object);
	}
	else {
		ABLOG(Error, TEXT("Can not load mesh!!! %s"), *AssetPath);
	}

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetBoxExtent(FVector(775.0F, 775.0F, 300.0F));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap);

	FString GateAssetPath = TEXT("StaticMesh'/Game/Book/StaticMesh/SM_GATE.SM_GATE'");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SMGate(*GateAssetPath);
	if (SMGate.Succeeded()) {
		static FName GetSockets[] = { {TEXT("+XGate")}, {TEXT("-XGate")}, {TEXT("+YGate")}, {TEXT("-YGate")} };
		for (FName Socket : GetSockets) {
			ABCHECK(Mesh->DoesSocketExist(Socket));

			// Set Door Mesh
			UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*Socket.ToString());
			NewGate->SetStaticMesh(SMGate.Object);
			NewGate->SetupAttachment(RootComponent, Socket);
			NewGate->SetRelativeLocation(FVector(0.0F, -80.5F, 0.0F));
			GateMeshes.Add(NewGate);

			// Set Door Trigger
			UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*Socket.ToString().Append(TEXT("_TRIGGER")));
			NewGateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
			NewGateTrigger->SetupAttachment(RootComponent, Socket);
			NewGateTrigger->SetRelativeLocation(FVector(70.0F, 0.0F, 250.0F));
			NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
			NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap);
			NewGateTrigger->ComponentTags.Add(Socket);

			GateTriggers.Add(NewGateTrigger);
		}
	}
	else {
		ABLOG(Error, TEXT("Failed to load gate!! %s"), *GateAssetPath);
	}

	bNoBattle = false;

	EnemySpawnTime = 2.0F;
	ItemBoxSpawnTime = 5.0F;
	SpawnNPCSize = 1;
}

// Called when the game starts or when spawned
void AABSection::BeginPlay()
{
	Super::BeginPlay();
	
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

void AABSection::SetState(ESectionState NewState) {
	switch (NewState) {
		case ESectionState::READY: {
			Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
			for (UBoxComponent* Box : GateTriggers) {
				Box->SetCollisionProfileName(TEXT("NoCollision"));
			}
			OperateGates(true);
			break;
		}
		case ESectionState::BATTLE: {
			Trigger->SetCollisionProfileName(TEXT("NoCollision"));
			for (UBoxComponent* Box : GateTriggers) {
				Box->SetCollisionProfileName(TEXT("NoCollision"));
			}
			OperateGates(false);

			GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimerHandle, FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), EnemySpawnTime, false);
			GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimerHandle, 
				FTimerDelegate::CreateLambda([this]() -> void {
					FVector2D RandXY = FMath::RandPointInCircle(600.0F);
					GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.0F), FRotator::ZeroRotator);
			}),
				ItemBoxSpawnTime, false
			);
			break;
		}
		case ESectionState::COMPLETE: {
			Trigger->SetCollisionProfileName(TEXT("NoCollision"));
			for (UBoxComponent* Box : GateTriggers) {
				Box->SetCollisionProfileName(TEXT("ABTrigger"));
			}
			OperateGates(true);
			break;
		}
	}

	CurrentState = NewState;
}

void AABSection::OperateGates(bool bOpen) {
	for (UStaticMeshComponent* Gate : GateMeshes) {
		Gate->SetRelativeRotation(bOpen ? FRotator(0.0F, 90.0F, 0.0F) : FRotator::ZeroRotator);
	}
}

void AABSection::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedCompo, AActor* OtherActor, UPrimitiveComponent* OtherCompo, int32 OtherBodyIdx, bool bFromSwwep, const FHitResult& SweepResult) {
	if (CurrentState == ESectionState::READY) {
		SetState(ESectionState::BATTLE);
	}
}

void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedCompo, AActor* OtherActor, UPrimitiveComponent* OtherCompo, int32 OtherBodyIdx, bool bFromSwwep, const FHitResult& SweepResult) {
	if (OverlappedCompo->ComponentTags.Num() != 1) {
		ABLOG(Warning, TEXT("FYCK!! %d"), OverlappedCompo->ComponentTags.Num());
		return;
	}

	FName ComponentTag = OverlappedCompo->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	if (!Mesh->DoesSocketExist(SocketName)) return;

	FVector NewLocation = Mesh->GetSocketLocation(SocketName);
	AABCharacter* Player = Cast<AABCharacter>(OtherActor);

	if (Player != nullptr && Player->GetController()->IsPlayerController()) {
		// 플레이어가 조작 하고 있음.
		auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator);
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
	FCollisionObjectQueryParams ObjectQueryParam(FCollisionObjectQueryParams::InitType::AllObjects);
	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		ObjectQueryParam,
		FCollisionShape::MakeSphere(775.0F),
		CollisionQueryParam
	);

	if (bResult) {
		auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator);
	}
	else {
		ABLOG(Warning, TEXT("An error occurred."));
	}
}

void AABSection::OnNPCSpawn() {
	GetWorld()->GetTimerManager().ClearTimer(SpawnNPCTimerHandle);
	auto KeyNPC = GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector * 88.0F, FRotator::ZeroRotator);

	if (KeyNPC != nullptr) {
		KeyNPC->OnDestroyed.AddDynamic(this, &AABSection::OnKeyNPCDestoryed);
	}
}

void AABSection::OnKeyNPCDestoryed(AActor* DestoryedActor) {
	auto ABCharacter = Cast<AABCharacter>(DestoryedActor);
	ABCHECK(ABCharacter != nullptr);

	auto ABPlayerController = Cast<AABPlayerController>(ABCharacter->LastHitBy);
	if (ABPlayerController != nullptr) {
		SetState(ESectionState::COMPLETE);
	}

	auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
	if (ABGameMode != nullptr) {
		ABGameMode->AddScore(ABPlayerController);
	}
}
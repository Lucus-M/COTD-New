#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class COTD_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

public:
	void SpawnEnemies();
	UPROPERTY()
	int32 AliveEnemies = 0;

	int32 CurrentWave = 0;

	UFUNCTION()
	void OnEnemyKilled(AActor* DeadEnemy);

	void StartNextWave();
	// Enemy class to spawn (set in Blueprint)
	UPROPERTY(EditAnywhere, Category="Spawning")
	TSubclassOf<AActor> EnemyClass;

	// Number of enemies to spawn
	UPROPERTY(EditAnywhere, Category="Spawning")
	int32 NumberOfEnemies = 10;

	// Radius around spawner to spawn enemies
	UPROPERTY(EditAnywhere, Category="Spawning")
	float SpawnRadius = 1000.0f;
};
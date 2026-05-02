#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

/**
 * AEnemySpawner
 * Handles wave-based spawning of enemies and pickups.
 * Tracks alive enemies, wave progression, and total kills.
 */
UCLASS()
class COTD_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

public:
	/**
	 * Spawns a full wave of enemies and optional pickup.
	 */
	void SpawnEnemies();

	/** Number of enemies currently alive in the world */
	UPROPERTY()
	int32 AliveEnemies = 0;

	/** Current wave index */
	int32 CurrentWave = 0;

	/**
	 * Called when an enemy is killed.
	 * Responsible for updating counters and triggering next wave.
	 */
	UFUNCTION()
	void OnEnemyKilled(AActor* DeadEnemy);

	/**
	 * Starts the next wave after a delay.
	 */
	void StartNextWave();

	/** Enemy class to spawn for waves */
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> EnemyClass;

	/** Number of enemies spawned per wave */
	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 NumberOfEnemies = 10;

	/** List of enemy spawn points placed in the level */
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<AActor*> SpawnPoints;

	/** Total number of zombies killed across all waves */
	UPROPERTY(BlueprintReadOnly)
	int32 TotalZombiesKilled = 0;

	/** Pickup class to spawn (e.g. heart pickup) */
	UPROPERTY(EditAnywhere, Category="Spawning")
	TSubclassOf<AActor> PickupClass;

	/** Determines whether a pickup spawns each wave */
	UPROPERTY(EditAnywhere, Category="Spawning")
	bool bSpawnPickupPerWave = true;

	/** Spawn points used specifically for heart pickups */
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<AActor*> HeartSpawnPoints;
};
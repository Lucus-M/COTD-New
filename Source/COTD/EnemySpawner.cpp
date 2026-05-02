#include "EnemySpawner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	// Spawn first wave when game begins
	SpawnEnemies();
}

void AEnemySpawner::SpawnEnemies()
{
	// Validate required data before spawning
	if (!EnemyClass || SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemySpawner missing EnemyClass or SpawnPoints"));
		return;
	}

	// Initialize wave state
	AliveEnemies = NumberOfEnemies;
	CurrentWave++;

	// -------------------------
	// SPAWN ZOMBIES
	// -------------------------
	for (int32 i = 0; i < NumberOfEnemies; i++)
	{
		// Choose random enemy spawn point
		int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
		AActor* SpawnPoint = SpawnPoints[RandomIndex];

		if (!SpawnPoint) continue;

		// Get transform for spawning
		FVector SpawnLocation = SpawnPoint->GetActorLocation();
		FRotator SpawnRotation = SpawnPoint->GetActorRotation();

		// Spawn enemy actor
		AActor* Enemy = GetWorld()->SpawnActor<AActor>(
			EnemyClass,
			SpawnLocation,
			SpawnRotation
		);

		if (Enemy)
		{
			AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(Enemy);

			if (EnemyChar)
			{
				// Assign spawner reference for kill tracking
				EnemyChar->SetSpawner(this);

				// Randomize enemy type stats
				float ZombieSpeed = 150.f;
				float Roll = FMath::FRand();

				// 15% chance for fast zombie variant
				if (Roll <= 0.15f)
				{
					ZombieSpeed = 300.f;
					EnemyChar->MaxHealth = 200.f;
				}
				else
				{
					EnemyChar->MaxHealth = 100.f;
				}

				// Apply final stats
				EnemyChar->CurrentHealth = EnemyChar->MaxHealth;
				EnemyChar->GetCharacterMovement()->MaxWalkSpeed = ZombieSpeed;
			}
		}
	}

	// -------------------------
	// SPAWN PICKUP (ONE PER WAVE)
	// -------------------------
	if (bSpawnPickupPerWave && PickupClass && SpawnPoints.Num() > 0)
	{
		// Choose random heart spawn location
		int32 PickupIndex = FMath::RandRange(0, HeartSpawnPoints.Num() - 1);
		AActor* PickupSpawnPoint = HeartSpawnPoints[PickupIndex];

		if (PickupSpawnPoint)
		{
			// Spawn pickup actor at location
			GetWorld()->SpawnActor<AActor>(
				PickupClass,
				PickupSpawnPoint->GetActorLocation(),
				PickupSpawnPoint->GetActorRotation()
			);

			UE_LOG(LogTemp, Warning, TEXT("Pickup spawned this wave"));
		}
	}
}

void AEnemySpawner::OnEnemyKilled(AActor* DeadEnemy)
{
	// Decrease alive enemy counter
	AliveEnemies--;

	// Increase total kill counter
	TotalZombiesKilled++;

	UE_LOG(LogTemp, Warning, TEXT("Enemy died. Alive enemies left: %d"), AliveEnemies);

	// If wave is cleared, start next wave after delay
	if (AliveEnemies <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Respawning enemies"));

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&AEnemySpawner::StartNextWave,
			3.0f,
			false
		);
	}
}

void AEnemySpawner::StartNextWave()
{
	// Increase difficulty each wave
	NumberOfEnemies += 1;

	// Spawn next wave
	SpawnEnemies();
}
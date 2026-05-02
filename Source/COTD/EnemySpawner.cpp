//EnemyCharacter.cpp

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

	SpawnEnemies();
}

void AEnemySpawner::SpawnEnemies()
{
	if (!EnemyClass || SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemySpawner missing EnemyClass or SpawnPoints"));
		return;
	}

	AliveEnemies = NumberOfEnemies;
	CurrentWave++;

	// -------------------------
	// SPAWN ZOMBIES
	// -------------------------
	for (int32 i = 0; i < NumberOfEnemies; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
		AActor* SpawnPoint = SpawnPoints[RandomIndex];

		if (!SpawnPoint) continue;

		FVector SpawnLocation = SpawnPoint->GetActorLocation();
		FRotator SpawnRotation = SpawnPoint->GetActorRotation();

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
				EnemyChar->SetSpawner(this);

				float ZombieSpeed = 150.f;
				float Roll = FMath::FRand();

				if (Roll <= 0.15f)
				{
					ZombieSpeed = 300.f;
					EnemyChar->MaxHealth = 200.f;
				}
				else
				{
					EnemyChar->MaxHealth = 100.f;
				}

				EnemyChar->CurrentHealth = EnemyChar->MaxHealth;
				EnemyChar->GetCharacterMovement()->MaxWalkSpeed = ZombieSpeed;
			}
		}
	}

	// -------------------------
	// SPAWN PICKUP (1 PER WAVE)
	// -------------------------
	if (bSpawnPickupPerWave && PickupClass && SpawnPoints.Num() > 0)
	{
		int32 PickupIndex = FMath::RandRange(0, HeartSpawnPoints.Num() - 1);
		AActor* PickupSpawnPoint = HeartSpawnPoints[PickupIndex];

		if (PickupSpawnPoint)
		{
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
	AliveEnemies--;
	TotalZombiesKilled++;
	
	UE_LOG(LogTemp, Warning, TEXT("Enemy died. Alive enemies left: %d"), AliveEnemies);

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
	NumberOfEnemies += 1; 
	SpawnEnemies();
}
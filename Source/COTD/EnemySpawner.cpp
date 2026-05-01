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

				float ZombieSpeed = 150.f; // normal speed

				float Roll = FMath::FRand();

				if (Roll <= 0.15f) // 15% chance for fast zombie
				{
					// FAST ZOMBIE
					ZombieSpeed = 300.f;

					EnemyChar->MaxHealth = 200.f;
					EnemyChar->CurrentHealth = EnemyChar->MaxHealth;

					UE_LOG(LogTemp, Warning, TEXT("FAST Zombie Spawned | Speed: %f | Health: %f"),
						ZombieSpeed,
						EnemyChar->MaxHealth);
				}
				else
				{
					// NORMAL ZOMBIE
					EnemyChar->MaxHealth = 100.f;
					EnemyChar->CurrentHealth = EnemyChar->MaxHealth;

					UE_LOG(LogTemp, Warning, TEXT("Normal Zombie Spawned | Speed: %f | Health: %f"),
						ZombieSpeed,
						EnemyChar->MaxHealth);
				}

				EnemyChar->GetCharacterMovement()->MaxWalkSpeed = ZombieSpeed;
			}
		}
	}
}
void AEnemySpawner::OnEnemyKilled(AActor* DeadEnemy)
{
	AliveEnemies--;

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
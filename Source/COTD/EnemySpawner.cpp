//EnemyCharacter.cpp

#include "EnemySpawner.h"

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
	if (!EnemyClass) return;

	AliveEnemies = NumberOfEnemies;
	CurrentWave++;

	for (int32 i = 0; i < NumberOfEnemies; i++)
	{
		FVector Origin = GetActorLocation();
		FVector RandomPoint = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SpawnRadius);
		FVector SpawnLocation = Origin + RandomPoint;

		FHitResult Hit;

		GetWorld()->LineTraceSingleByChannel(
			Hit,
			SpawnLocation + FVector(0,0,500),
			SpawnLocation - FVector(0,0,500),
			ECC_WorldStatic
		);

		FVector FinalLocation = Hit.bBlockingHit ? Hit.ImpactPoint : SpawnLocation;

		AActor* Enemy = GetWorld()->SpawnActor<AActor>(EnemyClass, FinalLocation, FRotator::ZeroRotator);

		if (Enemy)
		{
			AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(Enemy);
			if (EnemyChar)
			{
				EnemyChar->SetSpawner(this);
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
	NumberOfEnemies += 2; // optional difficulty scaling
	SpawnEnemies();
}
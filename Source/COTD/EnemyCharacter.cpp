#include "EnemyCharacter.h"
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "COTDCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);

    SpawnerRef = nullptr;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    GetCharacterMovement()->MaxWalkSpeed = 150.0f;

    CurrentHealth = MaxHealth;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDead) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

    AAIController* AIController = Cast<AAIController>(GetController());

    // CHASE (optimized - no spam MoveToActor every frame)
    if (AIController && Distance <= DetectionRange)
    {
        if (AIController->GetMoveStatus() != EPathFollowingStatus::Type::Moving)
        {
            AIController->MoveToActor(PlayerPawn, 10.0f);
        }
    }
    else if (AIController)
    {
        AIController->StopMovement();
    }

    // ATTACK PLAYER
    if (Distance <= AttackRange)
    {
        AttackingPlayer = true;

        float CurrentTime = GetWorld()->GetTimeSeconds();

        if (CurrentTime - LastAttackTime >= AttackCooldown)
        {
            LastAttackTime = CurrentTime;

            ACOTDCharacter* Player = Cast<ACOTDCharacter>(PlayerPawn);
            if (Player)
            {
                Player->TakeDamage(Damage);
            }
        }
    }
    else
    {
        AttackingPlayer = false;
    }
}

void AEnemyCharacter::SetSpawner(AEnemySpawner* Spawner)
{
    SpawnerRef = Spawner;
}

void AEnemyCharacter::TakeDamage(float DamageAmount)
{
    if (bIsDead) return;

    CurrentHealth -= DamageAmount;
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

    if (CurrentHealth <= 0.f)
    {
        bIsDead = true;

        if (SpawnerRef)
        {
            SpawnerRef->OnEnemyKilled(this);
            UE_LOG(LogTemp, Warning, TEXT("Enemy died"));
        }

        if (DeathEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                DeathEffect,
                GetActorLocation(),
                GetActorRotation()
            );
        }

        Destroy();
    }
}
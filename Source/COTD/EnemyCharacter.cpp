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

    // AI should rotate toward movement direction instead of controller rotation
    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);

    SpawnerRef = nullptr;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Set default movement speed
    GetCharacterMovement()->MaxWalkSpeed = 150.0f;

    // Initialize health
    CurrentHealth = MaxHealth;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Stop all logic if dead
    if (bIsDead) return;

    // Get player reference
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    // Calculate distance to player
    float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

    // Get AI controller
    AAIController* AIController = Cast<AAIController>(GetController());

    // -------------------------
    // CHASE LOGIC
    // -------------------------
    if (AIController && Distance <= DetectionRange)
    {
        // Only issue move command if not already moving
        if (AIController->GetMoveStatus() != EPathFollowingStatus::Type::Moving)
        {
            AIController->MoveToActor(PlayerPawn, 10.0f);
        }
    }
    else if (AIController)
    {
        // Stop movement if player is out of range
        AIController->StopMovement();
    }

    // -------------------------
    // ATTACK LOGIC
    // -------------------------
    if (Distance <= AttackRange)
    {
        AttackingPlayer = true;

        float CurrentTime = GetWorld()->GetTimeSeconds();

        // Attack only if cooldown has elapsed
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
        // Not in attack range
        AttackingPlayer = false;
    }
}

void AEnemyCharacter::SetSpawner(AEnemySpawner* Spawner)
{
    // Store reference to spawner for wave tracking
    SpawnerRef = Spawner;
}

void AEnemyCharacter::TakeDamage(float DamageAmount)
{
    // Ignore damage if already dead
    if (bIsDead) return;

    // Apply damage and clamp health
    CurrentHealth -= DamageAmount;
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
    Hurt();
    
    // Check for death
    if (CurrentHealth <= 0.f)
    {
        bIsDead = true;

        // Notify spawner
        if (SpawnerRef)
        {
            SpawnerRef->OnEnemyKilled(this);
            UE_LOG(LogTemp, Warning, TEXT("Enemy died"));
        }

        // Play death effect
        if (DeathEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                DeathEffect,
                GetActorLocation(),
                GetActorRotation()
            );
        }

        Die();
        // Remove enemy from world
        Destroy();
    }
}
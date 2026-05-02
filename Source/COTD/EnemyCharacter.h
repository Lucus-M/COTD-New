#pragma once
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class AEnemySpawner;

/**
 * AEnemyCharacter
 * Represents a single zombie AI unit.
 * Handles movement, chasing, attacking, health, and death effects.
 */
UCLASS()
class COTD_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

    /** Niagara effect played when enemy dies */
    UPROPERTY(EditDefaultsOnly, Category="Effects")
    UNiagaraSystem* DeathEffect;

    /** Distance at which enemy detects the player */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    float DetectionRange = 5000.0f;

    /** Damage dealt to player per attack */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float Damage = 10.0f;

    /** Distance required to start attacking player */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float AttackRange = 150.0f;

    /** Time delay between attacks */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float AttackCooldown = 1.0f;

    /** Maximum health of the enemy */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
    float MaxHealth = 100.f;

    /** Current health of the enemy */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
    float CurrentHealth = 100.f;

    /** Whether the enemy is currently attacking the player */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
    bool AttackingPlayer = false;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    /** Tracks last time an attack occurred */
    float LastAttackTime = 0.0f;

    /** Reference to the spawner that created this enemy */
    AEnemySpawner* SpawnerRef = nullptr;

    /** Assigns spawner reference for wave tracking */
    void SetSpawner(AEnemySpawner* Spawner);

    /** Applies damage to this enemy */
    void TakeDamage(float DamageAmount);
    
    UFUNCTION(BlueprintImplementableEvent, Category="SFX")
    void Die();
    
    UFUNCTION(BlueprintImplementableEvent, Category="SFX")
    void Hurt();

private:
    /** Whether this enemy is dead */
    bool bIsDead = false;
};
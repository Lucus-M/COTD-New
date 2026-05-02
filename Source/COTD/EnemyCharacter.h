#pragma once
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class AEnemySpawner;

UCLASS()
class COTD_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();
    UPROPERTY(EditDefaultsOnly, Category="Effects")
    UNiagaraSystem* DeathEffect;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    float DetectionRange = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float Damage = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float AttackCooldown = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
    float CurrentHealth = 100.f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
    bool AttackingPlayer = false;


protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    float LastAttackTime = 0.0f;

    // Wave system reference
    AEnemySpawner* SpawnerRef = nullptr;

    void SetSpawner(AEnemySpawner* Spawner);

    // Damage handling
    void TakeDamage(float DamageAmount);
    

private:
    bool bIsDead = false;
};
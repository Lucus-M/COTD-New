#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "COTDCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UUserWidget;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 * ACOTDCharacter
 * First-person player character handling movement, combat, health, dash, and UI.
 */
UCLASS(config=Game)
class ACOTDCharacter : public ACharacter
{
	GENERATED_BODY()

	/** First-person mesh (visible only to owner) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* Mesh1P;

	/** First-person camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Default input mapping context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* JumpAction;

	/** Movement input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* MoveAction;

	/** Look input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* LookAction;

	/** Dash input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* DashAction;

public:
	ACOTDCharacter();

	/** Blueprint event triggered when player takes damage */
	UFUNCTION(BlueprintImplementableEvent, Category="Health")
	void OnPlayerDamaged();

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	/** Performs dash movement */
	void Dash();

	/** Resets dash cooldown */
	void ResetDash();

	/** Damage dealt by player attacks */
	UPROPERTY(EditAnywhere, Category="Combat")
	float Damage = 10.0f;

	/** Attack range */
	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackRange = 150.0f;

	/** Attack cooldown time */
	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackCooldown = 1.0f;

	/** Tracks last attack time */
	float LastAttackTime = 0.0f;

public:
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/** Returns first-person mesh */
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns camera component */
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Maximum player health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float MaxHealth = 100.0f;

	/** Current player health */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float Health;

	/** Whether player is dead */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	bool bIsDead = false;

	/** Applies damage to player */
	UFUNCTION(BlueprintCallable, Category="Health")
	void TakeDamage(float DamageAmount);

	/** Returns whether player is dead */
	UFUNCTION(BlueprintCallable, Category="Health")
	bool IsDead() const;

	/** Dash strength */
	UPROPERTY(EditAnywhere, Category="Movement")
	float DashStrength = 3000.0f;

	/** Dash cooldown */
	UPROPERTY(EditAnywhere, Category="Movement")
	float DashCooldown = 1.0f;

	/** Whether player can currently dash */
	bool bCanDash = true;

	/** Timer handle for dash cooldown */
	FTimerHandle DashTimerHandle;

	/** Game over UI widget class */
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	/** Active game over widget instance */
	UUserWidget* GameOverWidget;

	/** Heals the player */
	UFUNCTION(BlueprintCallable, Category="Health")
	void Heal(float HealAmount);
};
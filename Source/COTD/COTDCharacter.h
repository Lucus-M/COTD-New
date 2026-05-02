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

UCLASS(config=Game)
class ACOTDCharacter : public ACharacter
{
	GENERATED_BODY()
	

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* DashAction;

public:
	ACOTDCharacter();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Health")
	void OnPlayerDamaged();

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Dash();
	void ResetDash();

	UPROPERTY(EditAnywhere, Category="Combat")
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackCooldown = 1.0f;

	float LastAttackTime = 0.0f;

public:
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	// HEALTH
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	bool bIsDead = false;

	UFUNCTION(BlueprintCallable, Category="Health")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category="Health")
	bool IsDead() const;

	// DASH
	UPROPERTY(EditAnywhere, Category="Movement")
	float DashStrength = 3000.0f;

	UPROPERTY(EditAnywhere, Category="Movement")
	float DashCooldown = 1.0f;

	bool bCanDash = true;

	FTimerHandle DashTimerHandle;

	// GAME OVER UI
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UUserWidget* GameOverWidget;
};
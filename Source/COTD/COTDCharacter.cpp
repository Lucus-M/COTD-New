#include "COTDCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ACOTDCharacter::ACOTDCharacter()
{
	// Set collision capsule size
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create first-person camera
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create first-person mesh
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Initialize health
	Health = MaxHealth;
}

void ACOTDCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add input mapping context when controller is assigned
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ACOTDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Bind Enhanced Input actions
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACOTDCharacter::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACOTDCharacter::Look);

		EIC->BindAction(DashAction, ETriggerEvent::Started, this, &ACOTDCharacter::Dash);
	}
}

void ACOTDCharacter::Move(const FInputActionValue& Value)
{
	if (bIsDead) return;

	FVector2D V = Value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), V.Y);
	AddMovementInput(GetActorRightVector(), V.X);
}

void ACOTDCharacter::Look(const FInputActionValue& Value)
{
	if (bIsDead) return;

	FVector2D V = Value.Get<FVector2D>();

	AddControllerYawInput(V.X);
	AddControllerPitchInput(V.Y);
}

void ACOTDCharacter::Dash()
{
	// Prevent dash if dead or on cooldown
	if (bIsDead || !bCanDash) return;

	bCanDash = false;

	// Determine dash direction
	FVector Dir = GetLastMovementInputVector();
	if (Dir.IsNearlyZero())
		Dir = GetActorForwardVector();

	// Apply movement impulse
	LaunchCharacter(Dir * DashStrength, true, true);

	// Reset dash after cooldown
	GetWorldTimerManager().SetTimer(
		DashTimerHandle,
		this,
		&ACOTDCharacter::ResetDash,
		DashCooldown,
		false
	);
}

void ACOTDCharacter::ResetDash()
{
	bCanDash = true;
}

void ACOTDCharacter::TakeDamage(float DamageAmount)
{
	// Ignore damage if already dead
	if (bIsDead) return;

	// Apply and clamp health
	Health -= DamageAmount;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);

	// Trigger blueprint event
	OnPlayerDamaged();

	// Check death condition
	if (Health <= 0.f)
	{
		bIsDead = true;

		// Disable movement
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->StopMovementImmediately();

		// Disable input and show UI
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->DisableInput(PC);

			if (GameOverWidgetClass)
			{
				GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
				if (GameOverWidget)
				{
					GameOverWidget->AddToViewport();

					FInputModeUIOnly Mode;
					PC->SetInputMode(Mode);
					PC->bShowMouseCursor = true;
				}
			}
		}

		UE_LOG(LogTemplateCharacter, Warning, TEXT("Player Died"));
	}
}

bool ACOTDCharacter::IsDead() const
{
	return bIsDead;
}

void ACOTDCharacter::Heal(float HealAmount)
{
	// Prevent healing if dead
	if (bIsDead) return;

	// Apply and clamp health
	Health += HealAmount;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);

	UE_LOG(LogTemplateCharacter, Warning,
		TEXT("Player Healed: %f | Current Health: %f"),
		HealAmount, Health);
}
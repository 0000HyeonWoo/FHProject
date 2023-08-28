// Copyright Epic Games, Inc. All Rights Reserved.

#include "FHProjectCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "BaseWeapon.h"
#include "WeaponInterface.h"


//////////////////////////////////////////////////////////////////////////
// AFHProjectCharacter

AFHProjectCharacter::AFHProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Set CapsuleComponent Visible
	GetCapsuleComponent()->bHiddenInGame = false;
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate


	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	// Set JumpZVelocity 700.f(Default) -> 350.f
	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->AirControl = 0.35f;
	//----------[ Default Speed Setting Here ]----------
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// ----------[ Add Setting ]----------
	// Set Oreint Rotation to Movement false to true
	UCharacterMovementComponent* const MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->bOrientRotationToMovement = true;
	}

	// C++ Rotate Pitch, Yaw, Roll
	// UE Rotate Roll, Pitch, Yaw

	// Set Character Mesh Location to Start Point of Capsule Collsion, Mesh looklike Standing on Ground
	// If do not this, Character Mesh floating in air
	FVector NewMeshLocation = { 0.0f, 0.0f, -90.0f };
	GetMesh()->SetRelativeLocation(NewMeshLocation);

	// Set Character Mesh Rotation, Make Character Mesh Look Forward
	// If do not this, Character Mesh always look right
	FRotator NewMeshRotation = { 0.0f, 270.0f, 0.0f };
	GetMesh()->SetRelativeRotation(NewMeshRotation);

	// Set Character Can Crouch
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	//Set Weapon Socket Name
	//If you want to change Socket Name, Edit like this -> FName(TEXT("MySocketName"))
	WeaponSocketName = FName(TEXT("Weapon"));

}

// Network Setting
void AFHProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFHProjectCharacter, PlayerRotation);
}

void AFHProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AFHProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If Has Authority, Set PlayerRotation Uproperty(Replicated)
	if (HasAuthority() == true)
	{
		PlayerRotation = GetControlRotation();
	}
}

float AFHProjectCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogClass, Warning, TEXT("ApplyDamage function Test :: TakeDamage"));

	UE_LOG(LogClass, Warning, TEXT("EventInstigator :: %s"), *FString(EventInstigator->GetName()));

	//Check Damage Value
	UE_LOG(LogClass, Warning, TEXT("DamageAmount :: %f"), DamageAmount);

	return 0.0f;
}

void AFHProjectCharacter::Req_DoRollMove_Implementation()
{
	//Client
	Res_DoRollMove();
}

void AFHProjectCharacter::Res_DoRollMove_Implementation()
{
	UE_LOG(LogClass, Warning, TEXT("DoRollMove"));

	// Play Target AnimMontage When Target AnimMontage Is not Playing
	if (bIsMontagePlaying() == true)
	{
		UE_LOG(LogClass, Warning, TEXT("bIsMontagePlaying::true"));
		return;
	}

	// If Character Is Falling = return
	if (GetCharacterMovement()->IsFalling() == true)
	{
		UE_LOG(LogClass, Warning, TEXT("IsFalling::true"));
		return;
	}

	// if Character Is Crouched = return
	if (bIsCrouched == true)
	{
		UE_LOG(LogClass, Warning, TEXT("bIsCrouched::true"));
		return;
	}

	// If StandToRollMontage Is Not Valid = return
	if (IsValid(StandToRollMontage) == false)
	{
		UE_LOG(LogClass, Warning, TEXT("IsValid::StandToRollMontage, false"));
		return;
	}

	// If RunToRollMontage Is Not Valid = return
	if (IsValid(RunToRollMontage) == false)
	{
		UE_LOG(LogClass, Warning, TEXT("IsValid::RunToRollMontage, false"));
		return;
	}

	// Check Max Speed And Play AnimMontage by Speed Value
	if (GetCharacterMovement()->GetMaxSpeed() <= 500.0f)
	{
		// Check Montage Is Playing
		//bIsMontagePlaying = GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();

		UE_LOG(LogClass, Warning, TEXT("PlayAnimMontage::StandToRollMontage"));
		PlayAnimMontage(StandToRollMontage);
	}
	else if (GetCharacterMovement()->GetMaxSpeed() > 500.0f)
	{
		// Check Montage Is Playing
		//bIsMontagePlaying = GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();

		UE_LOG(LogClass, Warning, TEXT("PlayAnimMontage::RunToRollMontage"));
		PlayAnimMontage(RunToRollMontage);
	}
}


void AFHProjectCharacter::Req_SetMaxWalkSpeed_Implementation(float NewSpeed)
{
	//Sprint and StopSprint Action Use This Function
	//Default Value 500.f
	//Walk = 500.0f, Sprint 750.0f
	UE_LOG(LogClass, Warning, TEXT("SetMaxWalkSpeed"));

	// Set MaxWalkSpeed New Speed - Server
	Res_SetMaxWalkSpeed(NewSpeed);
}

void AFHProjectCharacter::Res_SetMaxWalkSpeed_Implementation(float NewSpeed)
{
	// Set MaxWalkSpeed New Speed - Client
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AFHProjectCharacter::Res_AttachToWeaponSocket_Implementation(AActor* Item)
{
	UE_LOG(LogClass, Warning, TEXT("Res_AttachToWeaponSocket"));

	// EquipWeapon is Target Item
	EquipWeapon = Item;

	// Check EquipWeapon Is Valid
	if (IsValid(EquipWeapon) == true)
	{
		UE_LOG(LogClass, Warning, TEXT("IsValid::EquipWeapon"));
	}

	// Cast WeaponInterface - Item
	IWeaponInterface* WeaponInterfaceObj = Cast<IWeaponInterface>(Item);

	// Cast WeaponInterface pointer is nullptr = return
	if (WeaponInterfaceObj == nullptr)
	{
		return;
	}

	// Item's Event_AttachToComponent, Attach Target Character is Self
	WeaponInterfaceObj->Execute_Event_AttachToComponent(Item, this, WeaponSocketName);
}

void AFHProjectCharacter::Req_DropItem_Implementation()
{
	//Server
	Res_DropItem();
}

void AFHProjectCharacter::Res_DropItem_Implementation()
{
	//Client
	UE_LOG(LogClass, Warning, TEXT("Res_DropItem"));

	// Cast WeaponInterface - EquipWeapon
	IWeaponInterface* WeaponInterfaceObj = Cast<IWeaponInterface>(EquipWeapon);

	// Cast WeaponInterface pointer is nullptr = return
	if (WeaponInterfaceObj == nullptr)
	{
		return;
	}

	// Item's Event_DetachFromActor, Detach Target Character is Self
	WeaponInterfaceObj->Execute_Event_DetachFromActor(EquipWeapon, this);

	// Set EquipWeapon null
	EquipWeapon = nullptr;
}

void AFHProjectCharacter::Req_LeftClickAttack_Implementation(bool IsPressed)
{
	//Client
	Res_LeftClickAttack(IsPressed);
}

void AFHProjectCharacter::Res_LeftClickAttack_Implementation(bool IsPressed)
{
	UE_LOG(LogClass, Warning, TEXT("Res_LeftClickAttack"));

	// Cast WeaponInterface - EquipWeapon
	IWeaponInterface* WeaponInterfaceObj = Cast<IWeaponInterface>(EquipWeapon);

	// Cast WeaponInterface pointer is nullptr = return
	if (WeaponInterfaceObj == nullptr)
	{
		return;
	}

	WeaponInterfaceObj->Execute_Event_LeftClickAttack(EquipWeapon, IsPressed);

}

void AFHProjectCharacter::Req_RightClickAttack_Implementation(bool IsPressed)
{
	//Client
	Res_RightClickAttack(IsPressed);
}

void AFHProjectCharacter::Res_RightClickAttack_Implementation(bool IsPressed)
{
	UE_LOG(LogClass, Warning, TEXT("Res_RightClickAttack"));

	// Cast WeaponInterface - EquipWeapon
	IWeaponInterface* WeaponInterfaceObj = Cast<IWeaponInterface>(EquipWeapon);

	// Cast WeaponInterface pointer is nullptr = return
	if (WeaponInterfaceObj == nullptr)
	{
		return;
	}

	WeaponInterfaceObj->Execute_Event_RightClickAttack(EquipWeapon, IsPressed);
}

void AFHProjectCharacter::Event_GetItem_Implementation(EItemType eWeaponType, AActor* Item)
{
	UE_LOG(LogClass, Warning, TEXT("EventGetItem"));
	// WeaponInterface Event Switch
	switch (eWeaponType)
	{
	case EItemType::TestWeapon:
	{
		UE_LOG(LogClass, Warning, TEXT("EItemType::TestWeapon"));

		// Check Item is Valid
		if (IsValid(Item) == false)
		{
			UE_LOG(LogClass, Warning, TEXT("IsValid::Item == false"));

			return;
		}

		// Check Item ( Weapon ) has Owner Character
		ABaseWeapon* BaseWeaponObj = Cast<ABaseWeapon>(Item);
		if (BaseWeaponObj->GetOwnerCharacter() != nullptr)
		{
			UE_LOG(LogClass, Warning, TEXT("nullptr::Target Item has already has Owner Character"));
			return;
		}

		// Check EquipWeapon null
		if (EquipWeapon != nullptr)
		{
			UE_LOG(LogClass, Warning, TEXT("nullptr::Character has EquipWeapon"));

			return;
		}

		Res_AttachToWeaponSocket(Item);

		break;
	}
	
	}
}

FRotator AFHProjectCharacter::GetPlayerRotation()
{
	//Get Player Character index 0
	ACharacter* PlayerCharacter0 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	//If index 0 Player = Self, Get Controller Roatation
	if (PlayerCharacter0 == this)
	{
		return GetControlRotation();
	}

	return PlayerRotation;
}


//////////////////////////////////////////////////////////////////////////
// Input

void AFHProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFHProjectCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFHProjectCharacter::Look);

		//Roll
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AFHProjectCharacter::RollInput);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AFHProjectCharacter::SprintInput);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AFHProjectCharacter::StopSprintInput);

		//Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AFHProjectCharacter::CrouchInput);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AFHProjectCharacter::StopCrouchInput);

		//Drop Item
		EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Triggered, this, &AFHProjectCharacter::DropItemInput);

		//Attack - RightClick
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Triggered, this, &AFHProjectCharacter::RightClickInput);
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Completed, this, &AFHProjectCharacter::StopRightClickInput);

		//Attack - LeftClick
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Triggered, this, &AFHProjectCharacter::LeftClickInput);
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Completed, this, &AFHProjectCharacter::StopLeftClickInput);
	}

}

void AFHProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AFHProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFHProjectCharacter::RollInput(const FInputActionValue& Value)
{
	//Roll Action Input
	UE_LOG(LogClass, Warning, TEXT("RollInput"));

	//Server
	Req_DoRollMove();
}

void AFHProjectCharacter::SprintInput(const FInputActionValue& Value)
{
	//Sprint Action Input
	//If you want change Sprint Speed, Fix Value SetMaxWalkSpeed(here);
	UE_LOG(LogClass, Warning, TEXT("SprintInput"));

	//Server
	Req_SetMaxWalkSpeed(750.0f);
}

void AFHProjectCharacter::StopSprintInput(const FInputActionValue& Value)
{
	//StopSprint Action Input
	//If you want change Default Speed, Fix Value SetMaxWalkSpeed(here);
	//And Check AFHProjectCharacter(), GetCharacterMovement()->MaxWalkSpeed = here;
	UE_LOG(LogClass, Warning, TEXT("StopSprintInput"));

	//Server
	Req_SetMaxWalkSpeed(500.0f);
}

void AFHProjectCharacter::CrouchInput(const FInputActionValue& Value)
{
	//Crouch Action Input
	UE_LOG(LogClass, Warning, TEXT("CrouchInput"));
	//bIsMontagePlaying = GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();

	// Any Montage Is Playing = return
	if (bIsMontagePlaying() == true)
	{
		return;
	}

	// If Character Is Falling = return 
	if (GetCharacterMovement()->IsFalling() == true)
	{
		return;
	}

	// Do Crouch Move
	Crouch();
	
}

void AFHProjectCharacter::StopCrouchInput(const FInputActionValue& Value)
{
	//StopCrouch Action Input
	UE_LOG(LogClass, Warning, TEXT("StopCrouchInput"));
	//bIsMontagePlaying = GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();

	// Any Montage Is Playing = return
	if (bIsMontagePlaying() == true)
	{
		return;
	}

	// If Character Is Falling = return 
	if (GetCharacterMovement()->IsFalling() == true)
	{
		return;
	}
	
	// Do UnCrouch Move
	UnCrouch();
	
}

void AFHProjectCharacter::DropItemInput(const FInputActionValue& Value)
{
	//Drop Item Action Input
	UE_LOG(LogClass, Warning, TEXT("DropItemInput"));

	//Server
	Req_DropItem();
}

void AFHProjectCharacter::RightClickInput(const FInputActionValue& Value)
{
	//Right Click Input
	UE_LOG(LogClass, Warning, TEXT("RightClickInput"));

	//Check Character has EquipWeapon
	if (EquipWeapon == nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("nullptr::Character EquipWeapon is null"));

		return;
	}

	//Server
	//IsPressed is true
	Req_RightClickAttack(true);
}

void AFHProjectCharacter::StopRightClickInput(const FInputActionValue& Value)
{
	//Stop Right Click Input
	UE_LOG(LogClass, Warning, TEXT("StopRightClickInput"));

	//Check Character has EquipWeapon
	if (EquipWeapon == nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("nullptr::Character EquipWeapon is null"));

		return;
	}

	//Server
	//IsPressed is false
	Req_RightClickAttack(false);
}

void AFHProjectCharacter::LeftClickInput(const FInputActionValue& Value)
{
	//Left Click Input
	UE_LOG(LogClass, Warning, TEXT("LeftClickInput"));

	//Check Character has EquipWeapon
	if (EquipWeapon == nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("nullptr::Character EquipWeapon is null"));

		return;
	}

	//Server
	//IsPressed is true
	Req_LeftClickAttack(true);
}

void AFHProjectCharacter::StopLeftClickInput(const FInputActionValue& Value)
{
	//Stop Left Click Input
	UE_LOG(LogClass, Warning, TEXT("StopLeftClickInput"));

	//Check Character has EquipWeapon
	if (EquipWeapon == nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("nullptr::Character EquipWeapon is null"));

		return;
	}

	//UE_LOG(LogClass, Warning, TEXT("LeftClickCount :: %d"), LeftClickCount);

	//Server
	//IsPressed is false
	Req_LeftClickAttack(false);
}





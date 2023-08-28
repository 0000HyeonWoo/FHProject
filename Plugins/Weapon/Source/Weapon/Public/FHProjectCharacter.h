// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FHProjectCharacter.generated.h"


UCLASS(config=Game)
class AFHProjectCharacter : public ACharacter, public IWeaponInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

// ----------[ Add Setting ]----------
	/** Roll Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RollAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	/** Drop Item Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DropItemAction;

	/** RightClick Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RightClickAction;

	/** LeftClick Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LeftClickAction;


public:
	AFHProjectCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

// ----------[ Add Setting ]----------
	/** Called for Roll input */
	void RollInput(const FInputActionValue& Value);

	/** Called for Sprint input */
	void SprintInput(const FInputActionValue& Value);

	/** Called for StopSprint input */
	void StopSprintInput(const FInputActionValue& Value);

	/** Called for Crouch input */
	void CrouchInput(const FInputActionValue& Value);

	/** Called for StopCrouch input */
	void StopCrouchInput(const FInputActionValue& Value);

	/** Called for Drop Item input */
	void DropItemInput(const FInputActionValue& Value);

	/** Called for RightClick input */
	void RightClickInput(const FInputActionValue& Value);

	/** Called for StopRightClick input */
	void StopRightClickInput(const FInputActionValue& Value);

	/** Called for LeftClick input */
	void LeftClickInput(const FInputActionValue& Value);

	/** Called for StopLeftClick input */
	void StopLeftClickInput(const FInputActionValue& Value);



protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	// ----------[ Add Event ]----------
	// Tick override
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;



// ----------[ Add FUNCTION ]----------
public:
	// Do Roll Move function
	UFUNCTION(Server, Reliable)
	void Req_DoRollMove();

	UFUNCTION(NetMulticast, Reliable)
	void Res_DoRollMove();


	// Set MaxWalkSpeed For Sprint Action
	UFUNCTION(Server, Reliable)
	void Req_SetMaxWalkSpeed(float NewSpeed);

	UFUNCTION(NetMulticast, Reliable)
	void Res_SetMaxWalkSpeed(float NewSpeed);


	//Get Weapon Static Mesh And Attach to Target Socket
	UFUNCTION(NetMulticast, Reliable)
	void Res_AttachToWeaponSocket(AActor* Item);


	//Drop Item Attached on Target Socket
	UFUNCTION(Server, Reliable)
	void Req_DropItem();

	UFUNCTION(NetMulticast, Reliable)
	void Res_DropItem();


	//Left Click Attack Action
	UFUNCTION(Server, Reliable)
	void Req_LeftClickAttack(bool IsPressed);

	UFUNCTION(NetMulticast, Reliable)
	void Res_LeftClickAttack(bool IsPressed);


	//Right Click Attack Action
	UFUNCTION(Server, Reliable)
	void Req_RightClickAttack(bool IsPressed);

	UFUNCTION(NetMulticast, Reliable)
	void Res_RightClickAttack(bool IsPressed);


public:
	//WeaponInterface Event
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Event_GetItem(EItemType eWeaponType, AActor* Item);

	void Event_GetItem_Implementation(EItemType eWeaponType, AActor* Item) override;


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


// ----------[ Add PROPERTY ]----------

protected:
	//EquipWeapon
	AActor* EquipWeapon;

	//Character Mesh's Weapon Socket Name
	FName WeaponSocketName;


public:
	// Check Any MontagePlaying
	UFUNCTION(BlueprintCallable)
	bool bIsMontagePlaying() { return GetMesh()->GetAnimInstance()->IsAnyMontagePlaying(); };

	//Return Character's EquipWeapon
	AActor* GetEquipWeapon() { return EquipWeapon; };

public:
	// Use When Replicate Move Server and Client
	UPROPERTY(Replicated)
	FRotator PlayerRotation;

	//Return Player index 0's Rotation or just PlayerRotation
	UFUNCTION(BlueprintPure)
	FRotator GetPlayerRotation();

public:
	// Use When Roll
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* StandToRollMontage;

	// Use When Roll
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* RunToRollMontage;

};


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"


enum class EItemType : uint8;

UCLASS()
class WEAPON_API ABaseWeapon : public AActor, public IWeaponInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void MeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	//Interface Event
	//Get Item and Attach Item to Target Character's Socket
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Event_AttachToComponent(ACharacter* TargetCharacter, const FName& TargetSocketName);

	virtual void Event_AttachToComponent_Implementation(ACharacter* TargetCharacter, const FName& TargetSocketName) override;

	//Detach Item From Target Character's Socket
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Event_DetachFromActor(ACharacter* TargetCharacter);

	virtual void Event_DetachFromActor_Implementation(ACharacter* TargetCharacter) override;

	// Event Left Click Attack
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Event_LeftClickAttack(bool IsPressed);

	virtual void Event_LeftClickAttack_Implementation(bool IsPressed) override;

	// Event Right Click Attack
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Event_RightClickAttack(bool IsPressed);

	virtual void Event_RightClickAttack_Implementation(bool IsPressed) override;


protected:
	//Owner Character
	UPROPERTY(BlueprintReadWrite)
	ACharacter* OwnerCharacter;

	//Add Count When Completed Left Click Attack, Reset Count When Right Click Attack
	int LeftClickCount;

	//Damage Value
	int ClickAttackDamage;

	//Right Click Damage Limit Value
	float MaxRightClickDamage;


public:
	//Return OwnerCharacter
	ACharacter* GetOwnerCharacter() { return OwnerCharacter; };


	//Return LeftClickCount
	int GetLeftClickCount() { return LeftClickCount; };

	//Add LeftClickCount
	void AddLeftClickCount() { LeftClickCount += 1; };

	//Initialize LeftClickCount
	void InitializeLeftClickCount() { LeftClickCount = 0; };


	//Get Click Attack Damage
	int GetClickAttackDamage() { return ClickAttackDamage; };
	
	//Set Click Attack Damage
	void SetClickAttackDamage(int NewClickAttackDamage) { ClickAttackDamage = NewClickAttackDamage; };


	//Set Max Right Click Damage
	void SetMaxRightClickDamage(float NewMaxRightClickDamage) { MaxRightClickDamage = NewMaxRightClickDamage; };

	//Return MaxRightClickDamage
	float GetMaxRightClickDamage() { return MaxRightClickDamage; };


public:
	//Return Calculated Right Click Damage
	float GetCalculatedRightClickDamage();


public:
	//BaseWeapon has StaticMesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	UStaticMeshComponent* StaticMesh;

	//Enum Weapon Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	EItemType eWeaponType;
};

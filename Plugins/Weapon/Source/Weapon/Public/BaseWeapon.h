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
	//Test Function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Event_Test();

	virtual void Event_Test_Implementation() override;

	UFUNCTION(Server, Reliable)
	void Req_TestFunction();


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

	// Event Click Attack
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Event_ClickAttack();

	virtual void Event_ClickAttack_Implementation() override;


protected:
	//Owner Character
	ACharacter* OwnerCharacter;

	//Add Count When Completed Left Click Attack, Reset Count When Right Click Attack
	int32 LeftClickCount;

	//Damage Value
	int32 ClickAttackDamage;

	//Right Click Damage Limit Value
	float MaxRightClickDamage;

	//Attack Point Socket Name
	FName AttackStartSocketName;
	FName AttackEndSocketName;

	//Attack Effect Spawn Socket Name
	FName AttackEffectSocketName;

	//Attack Effect Scale Value
	FVector AttackEffectScale;

	//Set Effect Scale Value use this
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EffectScaleValue;

	//Attack Sound Spawn Socket Name
	FName AttackSoundSocketName;

	//Set When Click Event Acive
	bool bIsLeftClick;

	//Use When SphereTrace Function
	float SphereRadius;

	//Check Weapon Attack Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRangeWeapon;

	//Use This Value Only Range Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange;


public:
	//Return OwnerCharacter
	ACharacter* GetOwnerCharacter() { return OwnerCharacter; };


	//Return LeftClickCount
	int32 GetLeftClickCount() { return LeftClickCount; };

	//Add LeftClickCount
	void AddLeftClickCount() { LeftClickCount += 1; };

	//Initialize LeftClickCount
	void InitializeLeftClickCount() { LeftClickCount = 0; };


	//Get Click Attack Damage
	int32 GetClickAttackDamage() { return ClickAttackDamage; };
	
	//Set Click Attack Damage
	void SetClickAttackDamage(int32 NewClickAttackDamage) { ClickAttackDamage = NewClickAttackDamage; };


	//Set Max Right Click Damage
	void SetMaxRightClickDamage(float NewMaxRightClickDamage) { MaxRightClickDamage = NewMaxRightClickDamage; };

	//Return MaxRightClickDamage
	float GetMaxRightClickDamage() { return MaxRightClickDamage; };

	//Return IsLeftClick Value
	bool GetIsLeftClick() { return bIsLeftClick; };


	//Set Attack Effect Scale Function (Effect Scale Value is Vector)
	void SetAttackEffectScale(float NewScaleValue) { AttackEffectScale = { NewScaleValue, NewScaleValue, NewScaleValue }; };


public:
	//Return Calculated Right Click Damage
	float GetCalculatedRightClickDamage();

	//Play AnimMontage, Target is Weapon's OwnerCharacter
	void PlayAttackAnimMontage(UAnimMontage* TargetAttackMontage);

	//Apply Damage to Actor Class
	UFUNCTION(Server, Reliable)
	void Req_ApplyDamageToTargetActor(FVector Start, FVector End, float Damage);


public:
	// BaseWeapon has StaticMesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	UStaticMeshComponent* StaticMesh;

	// Enum Weapon Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	EItemType eWeaponType;

	// Use When Attack - Left Click
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AttackMontage;

	// Use When Special Attack - Right Click
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* SpecialAttackMontage;

	// Sound Cue
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* AttackSound;

	// Attack Effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* AttackEffect;

};

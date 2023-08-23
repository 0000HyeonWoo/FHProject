// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	TestWeapon UMETA(DisplayName = "TestWeapon"),
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WEAPON_API IWeaponInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Event Character Get Item
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Event)
	void Event_GetItem(EItemType WeaponType, AActor* Item);

	// Event Attach To Character's WeaponSocket
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Event)
	void Event_AttachToComponent(ACharacter* TargetCharacter);

	// Event Detach From Character's WeaponSocket
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Event)
	void Event_DetachFromActor(ACharacter* TargetCharacter);

	// Event Left Click Attack
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Event)
	void Event_LeftClickAttack(bool IsPressed);
};

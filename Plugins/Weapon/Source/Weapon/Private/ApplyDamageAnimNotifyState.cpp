// Fill out your copyright notice in the Description page of Project Settings.


#include "ApplyDamageAnimNotifyState.h"
#include "DrawDebugHelpers.h"
#include "FHProjectCharacter.h"
#include "BaseWeapon.h"


UApplyDamageAnimNotifyState::UApplyDamageAnimNotifyState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShouldFireInEditor = false;
}

void UApplyDamageAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	UE_LOG(LogClass, Warning, TEXT("NotifyBegin"));

	AFHProjectCharacter* FHProjectCharacterObj = Cast<AFHProjectCharacter>(MeshComp->GetOwner());
	if (FHProjectCharacterObj == nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("nullptr:FHProjectCharacterObj, true"));
		return;
	}
	
	EquipWeapon = FHProjectCharacterObj->GetEquipWeapon();

	BaseWeaponObj = Cast<ABaseWeapon>(EquipWeapon);
	if (BaseWeaponObj == nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("nullptr:BaseWeaponObj, true"));
		return;
	}

}

void UApplyDamageAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	UE_LOG(LogClass, Warning, TEXT("NotifyTick"));

	BaseWeaponObj->Execute_Event_ClickAttack(EquipWeapon);
}

void UApplyDamageAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UE_LOG(LogClass, Warning, TEXT("NotifyEnd"));
}

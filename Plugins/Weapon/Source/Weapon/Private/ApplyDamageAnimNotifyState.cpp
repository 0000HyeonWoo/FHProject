// Fill out your copyright notice in the Description page of Project Settings.


#include "ApplyDamageAnimNotifyState.h"
#include "WeaponInterface.h"
#include "BaseWeapon.h"


void UApplyDamageAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	UE_LOG(LogClass, Warning, TEXT("NotifyBegin"));
}

void UApplyDamageAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	UE_LOG(LogClass, Warning, TEXT("NotifyTick"));
	UE_LOG(LogClass, Warning, TEXT("MeshComp Owner Name :: %s"), *MeshComp->GetName());

	
	

	//CharacterObj->Execute_Event_Test(CharacterObj->GetEquipWeapon());

}

void UApplyDamageAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	UE_LOG(LogClass, Warning, TEXT("NotifyEnd"));
}

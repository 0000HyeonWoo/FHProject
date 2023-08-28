// Fill out your copyright notice in the Description page of Project Settings.


#include "ApplyDamageAnimNotifyState.h"

#include "DrawDebugHelpers.h"


UApplyDamageAnimNotifyState::UApplyDamageAnimNotifyState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShouldFireInEditor = false;
}

void UApplyDamageAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	UE_LOG(LogClass, Warning, TEXT("NotifyBegin"));

	//Set Attack Socket Name
	//If you want to change Socket Name, Edit like this -> FName(TEXT("MySocketName"))
	AttackStartSocketName = FName(TEXT("Attack_Start"));
	AttackEndSocketName = FName(TEXT("Attack_End"));

	
	TestL = MeshComp->GetSocketLocation(AttackStartSocketName);
	TestL2 = MeshComp->GetSocketLocation(AttackEndSocketName);

}

void UApplyDamageAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	UE_LOG(LogClass, Warning, TEXT("NotifyTick"));


	DrawDebugLine
	(
		MeshComp->GetOwner()->GetWorld(),
		TestL,
		TestL2,
		FColor::Yellow, false, 5.0f
	);
}

void UApplyDamageAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	UE_LOG(LogClass, Warning, TEXT("NotifyEnd"));
}

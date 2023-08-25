// Fill out your copyright notice in the Description page of Project Settings.


#include "ApplyDamageAnimNotifyState.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Math/Vector.h"
#include "Math/TransformNonVectorized.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"


UApplyDamageAnimNotifyState::UApplyDamageAnimNotifyState()
{
	
}

void UApplyDamageAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	UE_LOG(LogClass, Warning, TEXT("NotifyBegin"));

	//Set Attack Socket Name
	//If you want to change Socket Name, Edit like this -> FName(TEXT("MySocketName"))
	AttackStartSocketName = FName(TEXT("Attack_Start"));
	AttackEndSocketName = FName(TEXT("Attack_End"));
}

void UApplyDamageAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	UE_LOG(LogClass, Warning, TEXT("NotifyTick"));

	const USkeletalMeshSocket* AttackStart = MeshComp->GetSocketByName(AttackStartSocketName);
	if (IsValid(AttackStart) == false)
	{
		UE_LOG(LogClass, Warning, TEXT("IsValid::AttackStart, false"));
		return;
	}

	const USkeletalMeshSocket* AttackEnd = MeshComp->GetSocketByName(AttackEndSocketName);
	if (IsValid(AttackStart) == false)
	{
		UE_LOG(LogClass, Warning, TEXT("IsValid::AttackEnd, false"));
		return;
	}


	FVector TestL;
	FVector TestL2;

	FTransform TestT;
	FTransform TestT2;

	TestT = AttackStart->GetSocketLocalTransform();
	TestT2 = AttackEnd->GetSocketLocalTransform();

	TestL = TestT.GetLocation();
	TestL2 = TestT2.GetLocation();

	DrawDebugLine
	(
		GetWorld(),
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

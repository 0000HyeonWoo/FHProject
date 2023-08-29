// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "WeaponInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"



// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setting Static Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(StaticMesh);
	StaticMesh->SetCollisionProfileName("Weapon");
	StaticMesh->SetSimulatePhysics(true);

	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::MeshBeginOverlap);

	//Server Replicate Setting
	bReplicates = true;
	SetReplicateMovement(true);

	//Initialize LeftClickCount, int Type
	LeftClickCount = 0;
	
	//Initialize ClickAttackDamage, int Type
	ClickAttackDamage = 0;
	SetClickAttackDamage(10);

	//Initialize ClickAttackDamage, float Type
	MaxRightClickDamage = 0;
	SetMaxRightClickDamage(GetClickAttackDamage() * 1.5f);

	//Set Weapon Attack Type
	bIsRangeWeapon = true;

	AttackRange = 1000.f;

	//Set Attack Socket Name
	//If you want to change Socket Name, Edit like this -> FName(TEXT("MySocketName"))
	AttackStartSocketName = FName(TEXT("Attack_Start"));
	AttackEndSocketName = FName(TEXT("Attack_End"));

	//Set Sphere Radiut for Trace
	//Set Value by Weapon's StaticMesh Attack Parts
	SphereRadius = 32.f;

}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseWeapon::MeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogClass, Warning, TEXT("MeshBeginOverlap"));

	// BaseWeapon -> ~Weapon(BaseWeapon) -> Begin Play -> Set Enum Type ->  Event(Enum Type) ->
	// Character(Interface Event) -> Enum Switch -> Cast -> Cast Target Event

	// BaseWeapon -> Weapon ex1 -> ex1.BeginPlay -> Set Enum Type ex1, Interface Event, Destroy
	// Character(Interface Event) -> ....

	// If HasAuthority is false = return
	if (HasAuthority() == false)
	{
		return;
	}

	// Check Overlaped Actor has ( IWeaponInterface ) 
	IWeaponInterface* WeaponInterfaceObj = Cast<IWeaponInterface>(OtherActor);

	// If ( WeaponInterfaceObj ) is nullptr = return
	if (WeaponInterfaceObj == nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("WeaponInterfaceObj::nullptr"));
		return;
	}

	UE_LOG(LogClass, Warning, TEXT("Execute_EventGetItem"));
	WeaponInterfaceObj->Execute_Event_GetItem(OtherActor, eWeaponType, this);

	// If Actor Destroyed, Character's AttachToComponent function doesn't work
	//Destroy();

	UE_LOG(LogClass, Warning, TEXT("Character Name :: %s"), *OtherActor->GetName());

}

void ABaseWeapon::Event_Test_Implementation()
{
	//Server
	Req_TestFunction();
}

void ABaseWeapon::Req_TestFunction_Implementation()
{
	//Test Function
	UE_LOG(LogClass, Warning, TEXT("Event_Test"));
}

void ABaseWeapon::Event_AttachToComponent_Implementation(ACharacter* TargetCharacter, const FName& TargetSocketName)
{
	UE_LOG(LogClass, Warning, TEXT("Event_AttachToComponent"));

	// Set Owner Character
	OwnerCharacter = TargetCharacter;

	// SetSimulatePhysics false Because StaticMesh will attach to Target Socket
	StaticMesh->SetSimulatePhysics(false);

	// And Attach to Target Component Name ( FName("weapon") ) on Character Mesh
	AttachToComponent(TargetCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetSocketName);

}

void ABaseWeapon::Event_DetachFromActor_Implementation(ACharacter* TargetCharacter)
{
	UE_LOG(LogClass, Warning, TEXT("Event_DetachFromActor"));

	// Set Owner Character null
	OwnerCharacter = nullptr;

	// SetSimulatePhysics true Because StaticMesh will Detach from Target Socket
	StaticMesh->SetSimulatePhysics(true);

	// Detach from Owner Character
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ABaseWeapon::Event_LeftClickAttack_Implementation(bool IsPressed)
{
	//Character Left Click Input Event Active this function
	UE_LOG(LogClass, Warning, TEXT("Event_LeftClickAttack"));

	if (IsPressed == true)
	{
		UE_LOG(LogClass, Warning, TEXT("IsPressed true"));

		//Check Weapon has OwnerCharacter
		if (OwnerCharacter == nullptr)
		{
			UE_LOG(LogClass, Warning, TEXT("OwnerCharacter::nullptr"));
			return;
		}

		//Check Any Montage Playing
		if (OwnerCharacter->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() == true)
		{
			UE_LOG(LogClass, Warning, TEXT("IsAnyMontagePlaying::true"));
			return;
		}

		// If AttackMontage Is Not Valid = return
		if (IsValid(AttackMontage) == false)
		{
			UE_LOG(LogClass, Warning, TEXT("IsValid::AttackMontage, false"));
			return;
		}

		//Add Left Click Count
		AddLeftClickCount();
		UE_LOG(LogClass, Warning, TEXT("LeftClickCount :: %d"), LeftClickCount);

		PlayAttackAnimMontage(AttackMontage);

		//Left Click is true
		bIsLeftClick = true;
	}
	else if (IsPressed == false)
	{
		UE_LOG(LogClass, Warning, TEXT("IsPressed false"));
	}

}

void ABaseWeapon::Event_RightClickAttack_Implementation(bool IsPressed)
{
	//Character Right Click Input Event Active this function
	UE_LOG(LogClass, Warning, TEXT("Event_RightClickAttack"));

	if (IsPressed == true)
	{
		UE_LOG(LogClass, Warning, TEXT("IsPressed true"));

		//Check Weapon has OwnerCharacter
		if (OwnerCharacter == nullptr)
		{
			UE_LOG(LogClass, Warning, TEXT("OwnerCharacter::nullptr"));
			return;
		}

		//Check Any Montage Playing
		if (OwnerCharacter->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() == true)
		{
			UE_LOG(LogClass, Warning, TEXT("IsAnyMontagePlaying::true"));
			return;
		}

		// If SpecialAttackMontage Is Not Valid = return
		if (IsValid(SpecialAttackMontage) == false)
		{
			UE_LOG(LogClass, Warning, TEXT("IsValid::SpecialAttackMontage, false"));
			return;
		}

		//float RightClickDamage;
		//RightClickDamage = GetCalculatedRightClickDamage();
		//UE_LOG(LogClass, Warning, TEXT("CalculatedRightClickDamage :: %d"), RightClickDamage);

		PlayAttackAnimMontage(SpecialAttackMontage);

		//Left Click is flase
		bIsLeftClick = false;
	}
	else if (IsPressed == false)
	{
		UE_LOG(LogClass, Warning, TEXT("IsPressed false"));
	}
}

void ABaseWeapon::Event_ClickAttack_Implementation()
{
	UE_LOG(LogClass, Warning, TEXT("Event_ClickAttack"));
	//Click Event

	//Set Start, End Point Location Vector by Socket Location, Target is Weapon Mesh's Socket
	FVector AttackStartLocation;
	AttackStartLocation = StaticMesh->GetSocketLocation(AttackStartSocketName);

	FVector AttackEndLocation;

	//Set Value by Weapon Attack Type
	if (bIsRangeWeapon == true)
	{
		UE_LOG(LogClass, Warning, TEXT("IsRangeWeapon, true"));

		//Range Weapon
		//Set Attack Range(Attack End Location) by Add Value(Vector Value, Use X Value Only)
		AttackEndLocation = AttackStartLocation + (OwnerCharacter->GetActorForwardVector() * AttackRange);
	}
	else
	{
		UE_LOG(LogClass, Warning, TEXT("IsRangeWeapon, false"));

		AttackEndLocation = StaticMesh->GetSocketLocation(AttackEndSocketName);
	}
	

	//Check Has Authority, UNetDriver Error Come from here
	if (HasAuthority() == false)
	{
		UE_LOG(LogClass, Warning, TEXT("HasAuthority == false"));
		return;
	}

	//Active Event by Left Click Value
	//Use Start, End Location is Same, Difference is only Damage
	if (GetIsLeftClick() == true)
	{
		UE_LOG(LogClass, Warning, TEXT("GetIsLeftClick == true"));

		//Left Click Damage Event Use Default Damage
		Req_ApplyDamageToTargetActor(AttackStartLocation, AttackEndLocation, GetClickAttackDamage());
	}
	else
	{
		UE_LOG(LogClass, Warning, TEXT("GetIsLeftClick == false"));

		//Right Click Damage Event Use Calculated Damage
		Req_ApplyDamageToTargetActor(AttackStartLocation, AttackEndLocation, GetCalculatedRightClickDamage());

		//After Active Right Click Event, Initialize LeftClickCount 0;
		InitializeLeftClickCount();
		UE_LOG(LogClass, Warning, TEXT("LeftClickCount :: %d"), LeftClickCount);

	}
}

float ABaseWeapon::GetCalculatedRightClickDamage()
{
	UE_LOG(LogClass, Warning, TEXT("GetCalculatedRightClickDamage"));
	//Calculate Right Click Damage

	//Create Return Value
	float RightClickDamage;
	RightClickDamage = 0;

	// ** Right Click Damage = Default Damage + ( Default Damage * (Click Count * 0.1f)) **
	// Default Damage = Click Attack Damage
	// (Click Count * 0.1f) -> 1 = 0.1, 2 = 0.2, 3 = 0.3,,,

	//RightClickDamage increase by LeftClickCount Value
	//Each Left Click increase Damage Value 10%
	RightClickDamage = GetClickAttackDamage() + (GetClickAttackDamage() * (GetLeftClickCount() * 0.1f));

	//Check Calculated Damage Value
	if (RightClickDamage > GetMaxRightClickDamage())
	{
		UE_LOG(LogClass, Warning, TEXT("RightClickDamage > GetMaxRightClickDamage"));
		UE_LOG(LogClass, Warning, TEXT("RightClickDamage Value Before Change:: %f"), RightClickDamage);
		//If Calculated Value bigger than Max Value, Set Calculated Value to Max Value
		RightClickDamage = GetMaxRightClickDamage();
	}
	
	//Check Calculated Value
	UE_LOG(LogClass, Warning, TEXT("RightClickDamage :: %f"), RightClickDamage);

	return RightClickDamage;
}

void ABaseWeapon::PlayAttackAnimMontage(UAnimMontage* TargetAttackMontage)
{
	UE_LOG(LogClass, Warning, TEXT("PlayAttackAnimMontage"));
	//Play Attack AnimMontage

	OwnerCharacter->PlayAnimMontage(TargetAttackMontage);
}

void ABaseWeapon::Req_ApplyDamageToTargetActor_Implementation(FVector Start, FVector End, float Damage)
{
	UE_LOG(LogClass, Warning, TEXT("ApplyDamageToTargetActor"));

	//Check Damage Value
	UE_LOG(LogClass, Warning, TEXT("Apply Damage :: %f"), Damage);

	//Trace Result Value
	bool bIsHit;
	bIsHit = true;

	//Set Collision for Trace Function
	FHitResult AttackHitResult;
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
	QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Vehicle);
	QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Destructible);

	//Start Trace by Weapon Type
	//Range Weapon is LineTrace, else Weapon SphereTrace
	if (bIsRangeWeapon == true)
	{
		UE_LOG(LogClass, Warning, TEXT("bIsRangeWeapon == true"));

		//Add Ignore Actor
		FCollisionQueryParams QueryParamsIgnoredActor;
		QueryParamsIgnoredActor.AddIgnoredActor(OwnerCharacter);
		QueryParamsIgnoredActor.AddIgnoredActor(this);

		//Trace by Location Value, Collision
		bIsHit = GetWorld()->LineTraceSingleByObjectType(AttackHitResult, Start, End, QueryParams, QueryParamsIgnoredActor);

		//DrawDebugLine for Check LineTrace Function is Working
		DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 5.0f);
	}
	else
	{
		UE_LOG(LogClass, Warning, TEXT("bIsRangeWeapon == false"));

		//Add Ignore Actor
		TArray<AActor*> IgnoreActors;
		//----------[ Check Ignore Character Here ]----------
		//IgnoreActors.Add(OwnerCharacter);
		IgnoreActors.Add(this);

		bool bTraceComplex;
		bTraceComplex = false;

		bool bIgnoreSelf;
		bIgnoreSelf = true;

		//Start SphereTrace
		//Color Red = Hit false, Green = Hit true
		bIsHit = UKismetSystemLibrary::SphereTraceSingle
		(
			GetWorld(),
			Start,
			End,
			SphereRadius,
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_OverlapAll_Deprecated),
			bTraceComplex,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			AttackHitResult,
			bIgnoreSelf,
			FColor::Red,
			FColor::Green,
			5.f
		);

	}

	//If Trace(Attack) can't hit Anything, return
	if (bIsHit == false)
	{
		UE_LOG(LogClass, Warning, TEXT("bIsHit == false"));
		return;
	}

	//Get Hit Actor
	AActor* HitTargetObj = Cast<AActor>(AttackHitResult.GetActor());

	//Check Hit Actor nullptr
	if (HitTargetObj == nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("HitTargetObj == nullptr"));
		return;
	}
	
	//Check Hit Actor's Name
	UE_LOG(LogClass, Warning, TEXT("Hit Actor :: %s"), *FString(HitTargetObj->GetName()));

	//Apply Damage to Hit Actor, This function Active Target's TakeDamage
	UGameplayStatics::ApplyDamage(HitTargetObj, Damage, OwnerCharacter->GetController(), this, UDamageType::StaticClass());

	//Check ApplyDamage End
	//UE_LOG(LogClass, Warning, TEXT("ApplyDamage End"));

}

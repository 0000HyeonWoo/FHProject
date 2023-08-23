// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "WeaponInterface.h"
#include "GameFramework/Character.h"


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

	UE_LOG(LogTemp, Log, TEXT("Character Name :: %s"), *OtherActor->GetName());

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
	}
	else if (IsPressed == false)
	{
		UE_LOG(LogClass, Warning, TEXT("IsPressed false"));

		UE_LOG(LogClass, Warning, TEXT("LeftClickCount :: %d"), LeftClickCount);

		PlayAttackAnimMontage();
	}

}

void ABaseWeapon::Event_RightClickAttack_Implementation(bool IsPressed)
{
	//Character Right Click Input Event Active this function
	UE_LOG(LogClass, Warning, TEXT("Event_RightClickAttack"));

	if (IsPressed == true)
	{
		UE_LOG(LogClass, Warning, TEXT("IsPressed true"));
	}
	else if (IsPressed == false)
	{
		UE_LOG(LogClass, Warning, TEXT("IsPressed false"));

		float RightClickDamage;
		RightClickDamage = GetCalculatedRightClickDamage();
		UE_LOG(LogClass, Warning, TEXT("CalculatedRightClickDamage :: %d"), RightClickDamage);

		//After Calculate Damage, Initialize LeftClickCount 0;
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
	UE_LOG(LogClass, Warning, TEXT("RightClickDamage :: %f"), RightClickDamage);

	//Check Calculated Damage Value
	if (RightClickDamage > GetMaxRightClickDamage())
	{
		//If Calculated Value bigger than Max Value, Set Calculated Value to Max Value
		RightClickDamage = GetMaxRightClickDamage();
	}
	UE_LOG(LogClass, Warning, TEXT("RightClickDamage Changed :: %f"), RightClickDamage);

	return RightClickDamage;
}

void ABaseWeapon::PlayAttackAnimMontage()
{
	UE_LOG(LogClass, Warning, TEXT("PlayAttackAnimMontage"));
	//Play Attack AnimMontage

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

	AddLeftClickCount();
	OwnerCharacter->PlayAnimMontage(AttackMontage);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "RgCharacter.h"

#include "DrawDebugHelpers.h"
//#include "RgAttributeComponent.h"
//#include "RgInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ARgCharacter::ARgCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;		// Turn direction is camera on spring arm direction
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetupAttachment(SpringArmComponent);

	/*
	InteractionComponent= CreateDefaultSubobject<URgInteractionComponent>("InteractionComponent");

	AttributeComponent = CreateDefaultSubobject<URgAttributeComponent>("AttributeComponent");
	*/
	
	GetCharacterMovement()->bOrientRotationToMovement = true;		// The character physically turns toward their direction of movement

	bUseControllerRotationYaw = false;			// Character can face in one direction, but that direction will not (necessarily) be controller forward

	AttackAnimDelay = 0.2f;
}

void ARgCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// Bind the OnHealthChanged delegate to the OnHealthChanged function
	// AttributeComponent->OnHealthChanged.AddDynamic(this, &ARgCharacter::OnHealthChanged);
}

// Called when the game starts or when spawned
void ARgCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARgCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	AddMovementInput(ControlRot.Vector(), Value);
}

void ARgCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	// X = Forward
	// Y = Right
	// Z = Up
	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, Value);
}

void ARgCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ARgCharacter::PrimaryAttack_Fire, AttackAnimDelay);
}

void ARgCharacter::PrimaryAttack_Fire()
{
	if(ensureMsgf(PrimaryProjectileClass, TEXT("Assign the Primary projectile to the Character")))
	{
		SpawnProjectile(PrimaryProjectileClass);
	}
	
}

void ARgCharacter::TeleportViaProjectile()
{
	if(ensureMsgf(TeleportationProjectileClass, TEXT("Assign the Teleportation projectile to the Character")))
	{
		SpawnProjectile(TeleportationProjectileClass);
	}
}

void ARgCharacter::TeleportViaProjectile_TimeElapsed()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ARgCharacter::PrimaryAttack_Fire, AttackAnimDelay);
}

void ARgCharacter::UnleashBlackhole()
{
	if(ensureMsgf(BlackholeProjectileClass, TEXT("Assign the Blackhole projectile to the Character")))
	{
		SpawnProjectile(BlackholeProjectileClass);
	}
	
}

void ARgCharacter::UnleashBlackhole_TimeElapsed()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ARgCharacter::PrimaryAttack_Fire, AttackAnimDelay);
}

void ARgCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	if(ensureAlways(ClassToSpawn))
	{
		FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.Instigator = this;

		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		//Ignore the player
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector TraceStart = CameraComponent->GetComponentLocation();
		FVector TraceEnd = CameraComponent->GetComponentLocation() + (GetControlRotation().Vector()*7777);

		FHitResult HitData;
		if(GetWorld()->SweepSingleByObjectType(HitData, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, Shape, CollisionParams))
		{
			TraceEnd = HitData.ImpactPoint;
			//DrawDebugSphere(GetWorld(),TraceEnd, 20.0f, 12, FColor::Red, false, 2.0f);
		}

		FRotator ProjectileRotation = FRotationMatrix::MakeFromX(TraceEnd-HandLocation).Rotator();
		FTransform SpawnTM = FTransform(ProjectileRotation, HandLocation);
		GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTM, SpawnParameters);
	}
}

// Called every frame
void ARgCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARgCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Inputs that handle player movement
	PlayerInputComponent->BindAxis("MoveForward", this, &ARgCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARgCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ARgCharacter::PrimaryInteract);

	// Inputs that handle attacks
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ARgCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &ARgCharacter::TeleportViaProjectile);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ARgCharacter::UnleashBlackhole);
}

void ARgCharacter::PrimaryInteract()
{
	// InteractionComponent->PrimaryInteract();
}
/*
void ARgCharacter::OnHealthChanged(URgAttributeComponent* OwningComponent, AActor* InstigatorActor, float NewHealth,
	float Delta, float HealthFraction)
{

	UE_LOG(LogTemp, Log, TEXT("Health Changed"));
	if(NewHealth<=0.0f)
	{
		
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		DisableInput(PlayerController);
	}
}
*/


// Fill out your copyright notice in the Description page of Project Settings.


#include "RgCharacter.h"

// Sets default values
ARgCharacter::ARgCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARgCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

}


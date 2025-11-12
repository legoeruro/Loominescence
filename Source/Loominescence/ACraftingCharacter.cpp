// Fill out your copyright notice in the Description page of Project Settings.


#include "ACraftingCharacter.h"

// Sets default values
ACraftingCharacter::ACraftingCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACraftingCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACraftingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACraftingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// TODO: consider if we wanna implement the logic here or in the controller file
// (currently in controller)
void ACraftingCharacter::UpdateHandPosition(const FVector& WorldTarget)
{
	HandRef->SetActorLocation(WorldTarget);
}



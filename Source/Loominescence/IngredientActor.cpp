// Fill out your copyright notice in the Description page of Project Settings.


#include "IngredientActor.h"

// Sets default values
AIngredientActor::AIngredientActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//
	// Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	// Mesh->SetupAttachment(RootComponent);
	//
	// Mesh->SetMobility(EComponentMobility::Movable);
}

void AIngredientActor::CleanupOnDestroy_Implementation(bool bUsed)
{
    UE_LOG(LogTemp, Error, TEXT("Please implement the the cleanup function"));
}

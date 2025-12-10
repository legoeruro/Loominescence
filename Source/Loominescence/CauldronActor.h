// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IngredientActor.h"
#include "PotionActor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Utils/MixingManager.h"
#include "CauldronActor.generated.h"

UCLASS()
class LOOMINESCENCE_API ACauldronActor : public AActor
{
	GENERATED_BODY()

public:
	ACauldronActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddUnlockRecipeToJournal(FMixRecipe Recipe);
	virtual void AddUnlockRecipeToJournal_Implementation(FMixRecipe Recipe);

protected:
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* CauldronMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* WaterMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UBoxComponent* DropZone = nullptr;   // Detects ingredient overlaps

	// State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cauldron")
	TArray<AIngredientActor*> CurrentIngredients;

	UPROPERTY()
	UMaterialInstanceDynamic* WaterMaterial = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cauldron")
	TArray<TSubclassOf<AIngredientActor>> StoredIngredientClasses;

	// Blueprint params
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cauldron")
	float EjectForce = 300.f; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cauldron")
	float EjectAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cauldron|Spawning")
	TSubclassOf<APotionActor> PotionBlueprint;

	// Functions
	UFUNCTION()
	void OnDropZoneOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						   bool bFromSweep, const FHitResult& SweepResult);

public:
	UFUNCTION(BlueprintCallable)
	void OnInteract();

	UFUNCTION(BlueprintCallable)
	FString GetStringListOfIngredients() const;

	void MixIngredients();

	// Shoots out agent from the cauldron (ingredients or potion)
	void EjectAgent(AActor* ThisActor);

	void UpdateWaterColor();
};
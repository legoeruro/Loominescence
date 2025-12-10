// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utils/GameTypes.h"
#include "IngredientActor.generated.h"

UCLASS()
class LOOMINESCENCE_API AIngredientActor : public AActor
{
	GENERATED_BODY()

public:
	AIngredientActor();

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ingredient")
	// UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ingredient")
	FName IngredientName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ingredient")
	EElementalType ElementalType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ingredient")
	FLinearColor LiquidColor;

	// This cleanup function should be implemented in blueprint to add logic to change inventory items
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ingredient")
	void CleanupOnDestroy(bool WasItemUsed);

	virtual void CleanupOnDestroy_Implementation(bool WasItemUsed);
};

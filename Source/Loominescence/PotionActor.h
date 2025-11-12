// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IngredientActor.h"
#include "GameFramework/Actor.h"
#include "PotionActor.generated.h"

UCLASS()
class LOOMINESCENCE_API APotionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APotionActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInstanceDynamic* DynamicMat;

	UPROPERTY(BlueprintReadOnly, Category="Potion")
	FPotionData PotionData;

	UFUNCTION(BlueprintCallable, Category="Potion")
	void InitializePotion(const FPotionData& Data);
};

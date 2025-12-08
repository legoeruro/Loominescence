// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "ItemInfo.generated.h"

USTRUCT(BlueprintType)
struct LOOMINESCENCE_API FInventoryItem
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bInInventory = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bStackable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemAmount = 1;

	// This property used to be in the original struct,
	// but this made it hard to resize and move the mesh.
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	// UStaticMesh* ItemMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bHealable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 HealAmount = 0;
};

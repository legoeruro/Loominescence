// Fill out your copyright notice in the Description page of Project Settings.


#include "PotionActor.h"

APotionActor::APotionActor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PotionMesh"));
	RootComponent = Mesh;
	Mesh->SetSimulatePhysics(false);
}

void APotionActor::InitializePotion(const FPotionData& Data)
{
	PotionData = Data;

	if (!DynamicMat && Mesh->GetMaterial(0))
	{
		DynamicMat = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
		Mesh->SetMaterial(0, DynamicMat);
	}

	if (DynamicMat)
	{
		DynamicMat->SetVectorParameterValue(TEXT("BaseColor"), Data.LiquidColor);
	}

	UE_LOG(LogTemp, Log, TEXT("Potion Created: %s (Effect: %d)"),
		*Data.PotionName.ToString(),
		static_cast<uint8>(Data.EffectType));
}

FString EnumToString(EPotionEffect Value)
{
	const UEnum* EnumPtr = StaticEnum<EPotionEffect>();
	if (!EnumPtr) return "Invalid";

	return EnumPtr->GetNameStringByValue((int64)Value);
}

FTooltipData APotionActor::GetTooltipData_Implementation() const
{
	FTooltipData Data;
	Data.RichText = FString::Printf(
		TEXT("<Title>Potion</>\n<Description>A potion with the effect %s</>"), *EnumToString(PotionData.EffectType)
	);
	return Data;
}


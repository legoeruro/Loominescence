#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IngredientActor.h"
#include "SpawnerBoxActor.generated.h"

UCLASS()
class LOOMINESCENCE_API ASpawnerBoxActor : public AActor
{
	GENERATED_BODY()

public:
	ASpawnerBoxActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BoxMesh;

	// class to spawn when pressed on box
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BoxVariables")
	TSubclassOf<AIngredientActor> SpawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BoxVariables")
	FLinearColor TintColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BoxVariables")
	float EjectForce = 300.f;

	UPROPERTY()
	UMaterialInstanceDynamic* BoxMaterial = nullptr;

	UFUNCTION(BlueprintCallable)
	void TriggerSpawn();

	void SpawnAndLaunch();

protected:
	virtual void BeginPlay() override;
};

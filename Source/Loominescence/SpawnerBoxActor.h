#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IngredientActor.h"
#include "SpawnerBoxActor.generated.h"

UCLASS(meta = (ToolTip = "This class is for box actors, which upon pressing spawns a material considering you have it in inventory."))
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
	
	// Which item should this box search for (and spawn one of)
	// Expect this to be unique
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BoxVariables")
	FString ItemNameToSpawn = TEXT("None");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BoxVariables")
	EElementalType ItemElementalType = EElementalType::None;

	UPROPERTY(BlueprintReadOnly, Category="BoxVariables")
	bool bIsBoxUnavailable = false;

	UPROPERTY()
	UMaterialInstanceDynamic* BoxMaterial = nullptr;

	UFUNCTION(BlueprintCallable)
	void TriggerSpawn();

	void SpawnAndLaunch();
	
	UFUNCTION(BlueprintCallable)
    void UpdateBoxAvailability();


protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	UActorComponent* InventoryComp;
};

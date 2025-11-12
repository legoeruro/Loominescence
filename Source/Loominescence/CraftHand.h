#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "CraftHand.generated.h"

UCLASS()
class LOOMINESCENCE_API ACraftHand : public AActor
{
	GENERATED_BODY()

public:
	ACraftHand();

protected:
	virtual void BeginPlay() override;

	// Properties from blueprint
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Hand")
	UStaticMeshComponent* HandMesh;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Hand")
	USphereComponent* CollisionSphere;

	// Properties and functions for the class
	UPROPERTY()
	AActor* GrabbedActor = nullptr;

	UPROPERTY()
	AActor* OverlappingActor = nullptr;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
						const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};

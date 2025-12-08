#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "CustomUI/TooltipWidget.h" 
#include "GameFramework/Actor.h"
#include "CraftHand.generated.h"

UCLASS()
class LOOMINESCENCE_API ACraftHand : public AActor
{
	GENERATED_BODY()

public:
	ACraftHand();
	
	UFUNCTION()
	void BeginGrab();
	
	UFUNCTION()
	void EndGrab();

	// Tooltip reference
	// The position of the tooltip will be updated through the controller in tick(), while the content and visibility
	// will be managed by the craft hand
	UPROPERTY()
	UTooltipWidgetBase* TooltipWidgetInstance;

	UPROPERTY()
	AActor* CurrentTooltipActor = nullptr;

protected:
	// Settings
	UPROPERTY(EditAnywhere, Category = "Hand")
	float ObjectZPLane = 200.f;
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	// Properties from blueprint
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Hand")
	UStaticMeshComponent* HandMesh;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Hand")
	USphereComponent* CollisionSphere;

	UPROPERTY()
	AActor* GrabbedActor = nullptr;

	UPROPERTY()
	AActor* OverlappingActor = nullptr;

	UPROPERTY()
	TArray<AActor*> OverlappingActors;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
						const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void UpdateClosestActor();

	UFUNCTION()
	bool DoesActorHaveTooltip(AActor* Actor);
	
	UFUNCTION()
	void UpdateTooltip(bool IsOn, AActor* Actor);
};

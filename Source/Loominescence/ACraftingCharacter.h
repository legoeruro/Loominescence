#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CraftHand.h"
#include "ACraftingCharacter.generated.h"

UCLASS()
class LOOMINESCENCE_API ACraftingCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACraftingCharacter();

	UFUNCTION()
	void UpdateHandPosition(const FVector& WorldTarget);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool bHoldingGrab = false;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Hand")
	TSubclassOf<ACraftHand> HandClass;

	UPROPERTY()
	ACraftHand* HandRef;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

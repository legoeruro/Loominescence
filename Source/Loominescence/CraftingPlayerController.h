// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CraftingPlayerController.generated.h"

UCLASS()
class LOOMINESCENCE_API ACraftingPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ACraftingPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // --- Input Mapping ---
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputMappingContext* CraftingMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* MouseMoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* GrabAction;

    // --- Hand Reference ---
    UPROPERTY(EditAnywhere, Category = "Hand")
    TSubclassOf<class AActor> HandActorClass;

    UPROPERTY()
    AActor* HandActor;

    UPROPERTY()
    AActor* LevelCamera;

    FVector LastMouseLocation = FVector::ZeroVector;

    // --- Settings ---
    UPROPERTY(EditAnywhere, Category = "Hand")
    float HandZPlane = 100.f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraFollowIntensity = 0.05f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraInterpSpeed = 5.f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraOffsetFactor = 0.05f;

private:
    void SetupInputMapping();
    void HandleGrab();
    void HandleMouseMove();
    void UpdateHandPosition();
    void UpdateCameraOffset(float DeltaSeconds);
};

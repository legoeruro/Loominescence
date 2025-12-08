// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingPlayerController.h"
#include "ACraftingCharacter.h"
#include "Camera/CameraActor.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ACraftingPlayerController::ACraftingPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    PrimaryActorTick.bCanEverTick = true;
}


void ACraftingPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Apply IMC to subsystem
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (CraftingMappingContext)
                Subsystem->AddMappingContext(CraftingMappingContext, 0);
        }
    }
    
    // Set camera from CameraActor
    TArray<AActor*> Cameras;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), Cameras);
    int cameraCount = Cameras.Num();
    if (Cameras.Num() > 0)
    {
        SetViewTargetWithBlend(Cameras[0]);
        LevelCamera = Cast<ACameraActor>(Cameras[0]);
    }
    
    if (!HandActor && HandActorClass)
    {
        FActorSpawnParameters Params;
        Params.Owner = this;
        HandActor = GetWorld()->SpawnActor<ACraftHand>(HandActorClass, FVector(0.f, 0.f, 0.f), FRotator::ZeroRotator, Params);
    }

    TooltipWidgetInstance = BP_CreateTooltipWidget();
    if (TooltipWidgetInstance && HandActor)
    {
        HandActor->TooltipWidgetInstance = TooltipWidgetInstance;
    }
}

void ACraftingPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateHandPosition();
    UpdateCameraOffset(DeltaSeconds);

    // TODO: consider raycasting (FHitResult) instead of overlapping
    // The actual visibility and content modification is done by CraftHand.cpp
    if (TooltipWidgetInstance)
    {
        FVector2D MousePos;
        if (GetMousePosition(MousePos.X, MousePos.Y))
        {
            // offset so it doesn't sit exactly under cursor
            const FVector2D Offset(16.f, 16.f);
            TooltipWidgetInstance->SetPositionInViewport(MousePos + Offset, true);
        }
    }
}

void ACraftingPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);

    // This sometimes does not work (unless live coding is triggered at least once)
    Input->BindAction(GrabAction, ETriggerEvent::Started, this, &ACraftingPlayerController::HandleBeginGrab);
    Input->BindAction(GrabAction, ETriggerEvent::Completed, this, &ACraftingPlayerController::HandleEndGrab);
    //Input->BindAction(GoToPlatformLevelAction, ETriggerEvent::Triggered, this, &ACraftingPlayerController::HandleGoToNewLevel);
}

void ACraftingPlayerController::HandleGoToNewLevel()
{
    UE_LOG(LogTemp, Log, TEXT("Go to new level"));

    //TODO: change name
    UGameplayStatics::OpenLevel(this, FName("Greybox"));
}

void ACraftingPlayerController::HandleBeginGrab()
{
    UE_LOG(LogTemp, Log, TEXT("Grab triggered"));
    HandActor->BeginGrab();
}

void ACraftingPlayerController::HandleEndGrab()
{
    UE_LOG(LogTemp, Log, TEXT("Grab released"));
    HandActor->EndGrab();
}

void ACraftingPlayerController::HandleMouseMove()
{
    UE_LOG(LogTemp, Log, TEXT("Mouse move triggered"));
}

void ACraftingPlayerController::UpdateHandPosition()
{
    if (!HandActor) return;

    float MouseX, MouseY;
    if (GetMousePosition(MouseX, MouseY))
    {
        FVector WorldOrigin, WorldDirection;
        DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDirection);

        const float Distance = (HandZPlane - WorldOrigin.Z) / WorldDirection.Z;
        FVector NewLocation = WorldOrigin + (WorldDirection * Distance);

        HandActor->SetActorLocation(NewLocation);

        // Move camera
        // TODO: Consider moving this to another place that makes more sense
        // TODO: clamp the values of offset & add smoothing
        FVector camLoc = LevelCamera->GetActorLocation();
        
        FVector mouseLocation = FVector(MouseX, MouseY, 0.0f);
        FVector Offset = -(mouseLocation - LastMouseLocation) * 0.05f; 
        LevelCamera->SetActorLocation(camLoc + Offset);

        LastMouseLocation = mouseLocation;
    }
}

void ACraftingPlayerController::UpdateCameraOffset(float DeltaSeconds)
{
    APlayerCameraManager* CamManager = PlayerCameraManager;
    if (!CamManager) return;

    FVector2D ViewportSize;
    GEngine->GameViewport->GetViewportSize(ViewportSize);

    float MouseX, MouseY;
    GetMousePosition(MouseX, MouseY);

    FVector2D Center = ViewportSize * 0.5f;
    FVector2D Offset = (FVector2D(MouseX, MouseY) - Center) * CameraFollowIntensity;

    FVector TargetLocation = FVector(Offset.X, Offset.Y, 0.f);
    FVector CurrentLoc = CamManager->GetCameraLocation();
    FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLocation, DeltaSeconds, CameraInterpSpeed);

    // CamManager->location
    CamManager->SetActorLocation(NewLoc);
}

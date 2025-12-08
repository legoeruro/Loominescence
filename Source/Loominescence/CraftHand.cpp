// CraftHand.cpp
#include "CraftHand.h"

#include "CauldronActor.h"
#include "SpawnerBoxActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "CustomUI/TooltipProvider.h"
#include "Utils/ULoomiUtils.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"

ACraftHand::ACraftHand()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACraftHand::BeginPlay()
{
    Super::BeginPlay();

    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionObjectType(ECC_Pawn);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    CollisionSphere->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
    
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACraftHand::OnBeginOverlap);
    CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ACraftHand::OnEndOverlap);
}

// TODO: change it so that this works
void ChangeHighlightActor(const AActor* GrabbedActor, bool IsHighlighted)
{
    if (!GrabbedActor) return;
    UPrimitiveComponent* MeshComp = Cast<UPrimitiveComponent>(GrabbedActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));
    if (MeshComp)
    {
        MeshComp->SetRenderCustomDepth(IsHighlighted);
    }
}

bool ACraftHand::DoesActorHaveTooltip(AActor* Actor)
{
    bool haveTooltip = Actor->GetClass()->ImplementsInterface(UTooltipProvider::StaticClass());
    if (!haveTooltip)
        UE_LOG(LogTemp, Warning, TEXT("Tooltip not implemented"));
    return haveTooltip;
}

void ACraftHand::UpdateTooltip(bool IsOn, AActor* Actor)
{
    if (!TooltipWidgetInstance->IsInViewport())
    {
        UE_LOG(LogTemp, Error, TEXT("TooltipWidgetInstance is NOT in viewport"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Updating tooltip."));
    UE_LOG(LogTemp, Warning, TEXT("Class is: %s (Class: %s)"),
        *Actor->GetName(),
        *Actor->GetClass()->GetName());

    if (!IsOn || !DoesActorHaveTooltip(Actor))
    {
        UE_LOG(LogTemp, Log, TEXT("No update"));
        TooltipWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    // We want to turn on tooltip and actor does have tooltip information
    FTooltipData Data =
        ITooltipProvider::Execute_GetTooltipData(Actor);
    UE_LOG(LogTemp, Log, TEXT("Tooltip new info: %s"), *Data.RichText);

    TooltipWidgetInstance->UpdateFromTooltipData(Data);
    TooltipWidgetInstance->SetVisibility(ESlateVisibility::Visible);
}



void ACraftHand::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Overlapping with: %s (Class: %s)"),
        *OtherActor->GetName(),
        *OtherActor->GetClass()->GetName());


    // If grabbing an object already --> do not execute overlap code
    if (GrabbedActor) return;
    
    OverlappingActors.AddUnique(OtherActor);

    UE_LOG(LogTemp, Warning, TEXT("Added overlap: %s"), *OtherActor->GetName());

    UpdateClosestActor();
}

void ACraftHand::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    
    if (GrabbedActor) return;
    
    OverlappingActors.Remove(OtherActor);
    UpdateClosestActor();
}

// Also updates the tooltip
void ACraftHand::UpdateClosestActor()
{
    float BestDistance = FLT_MAX;
    AActor* NewClosest = nullptr;

    FVector HandLocation = GetActorLocation();

    for (AActor* Actor : OverlappingActors)
    {
        if (!IsValid(Actor)) continue;

        float Dist = FVector::Dist(HandLocation, Actor->GetActorLocation());
        if (Dist < BestDistance)
        {
            BestDistance = Dist;
            NewClosest = Actor;
        }
    }

    if (NewClosest == nullptr)
    {
        if (OverlappingActor)
        {
            ChangeHighlightActor(OverlappingActor, false);
        }
        UpdateTooltip(false, OverlappingActor);
        return;
    }

    // If closest changed, update highlight
    if (NewClosest != OverlappingActor)
    {
        // Remove highlight from old closest
        if (OverlappingActor)
        {
            ChangeHighlightActor(OverlappingActor, false);
        }

        OverlappingActor = NewClosest;

        // Add highlight to new closest
        if (OverlappingActor)
        {
            ChangeHighlightActor(OverlappingActor, true);
        }
    }
    UpdateTooltip(true, OverlappingActor);
}

void ToggleActorPhysics(const AActor* GrabbedActor, bool IsPhysicsOn)
{
    if (!GrabbedActor) return;
    if (UStaticMeshComponent* MeshComp = GrabbedActor->FindComponentByClass<UStaticMeshComponent>())
    {
        MeshComp->SetSimulatePhysics(IsPhysicsOn);  // Turn on physics
    }
}

void ACraftHand::BeginGrab()
{
    if (GrabbedActor) return;
    if (!OverlappingActor) return;
    
    UE_LOG(LogTemp, Warning, TEXT("Grabbing: %s (Class: %s)"),
        *OverlappingActor->GetName(),
        *OverlappingActor->GetClass()->GetName());
    // Special case: grab onto the pot
    if (ACauldronActor* Cauldron = Cast<ACauldronActor>(OverlappingActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("This is a cauldron"));

        Cauldron->OnInteract();
        return;
    }

    if (ASpawnerBoxActor* SpawnerBox = Cast<ASpawnerBoxActor>(OverlappingActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("This is a Box"));
        SpawnerBox->TriggerSpawn();
        return;
    }

    if (APotionActor* Potion = Cast<APotionActor>(OverlappingActor))
    {
        // 1st, call function "Add Potion" from the inventory manager

        UE_LOG(LogTemp, Warning, TEXT("Grabbing a potion: %s"), *Potion->GetName());
    
        FPotionData PotionData = Potion->PotionData;
    
        
        UActorComponent* InventoryComp = ULoomiUtils::GetInventoryComponent(this);
        if (!InventoryComp)
        {
            UE_LOG(LogTemp, Error, TEXT("Inventory component not found!"));
            return;
        }
    
        static FName AddPotionFuncName = TEXT("AddPotion");
        UFunction* AddPotionFunction = InventoryComp->FindFunction(AddPotionFuncName);
    
        if (!AddPotionFunction)
        {
            UE_LOG(LogTemp, Error, TEXT("AddPotion() function not found on InventoryComp!"));
            return;
        }
    
        struct FAddPotion_Params
        {
            FPotionData Potion;
        };
    
        FAddPotion_Params Params;
        Params.Potion = PotionData;
    
        InventoryComp->ProcessEvent(AddPotionFunction, &Params);
    
        UE_LOG(LogTemp, Warning, TEXT("Potion added to inventory successfully."));
        
        // 2nd, destroy potion
        Potion->Destroy();
        return;
    }

    GrabbedActor = OverlappingActor;
    ToggleActorPhysics(GrabbedActor, false);
}

void ACraftHand::BeginRightMouse()
{
    // get overlapping actor
    // run cleanup code for overlapping actor (parsing it into an ingredient)
    // remove overlapping actor
    if (!OverlappingActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Right-click: No overlapping actor."));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Right-click on: %s (Class: %s)"),
        *OverlappingActor->GetName(),
        *OverlappingActor->GetClass()->GetName());

    if (AIngredientActor* Ingredient = Cast<AIngredientActor>(OverlappingActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cleaning up ingredient: %s"), *Ingredient->GetName());

        // Call Blueprint/C++ cleanup logic
        Ingredient->CleanupOnDestroy(true);

        // Add ingredient back to inventory
        UActorComponent* InventoryComp = ULoomiUtils::GetInventoryComponent(this);
        if (!InventoryComp)
        {
            UE_LOG(LogTemp, Error, TEXT("No inventory component found for ingredient cleanup!"));
            return;
        }

        static FName AddIngredientFuncName = TEXT("AddIngredient");
        UFunction* AddIngredientFunction = InventoryComp->FindFunction(AddIngredientFuncName);

        if (!AddIngredientFunction)
        {
            UE_LOG(LogTemp, Error, TEXT("AddIngredient() function not found on InventoryComp!"));
            return;
        }

        struct FAddIngredient_Params
        {
            FIngredientData IngredientData;  // You must define this inside IngredientActor
        };

        // Build params (your ingredient must expose something like Ingredient->IngredientData)
        FAddIngredient_Params Params;
        Params.IngredientData = Ingredient->IngredientData;  

        InventoryComp->ProcessEvent(AddIngredientFunction, &Params);

        UE_LOG(LogTemp, Warning, TEXT("Ingredient returned to inventory."));

        // Destroy actor after returning
        Ingredient->Destroy();
        return;
    }

}


void ACraftHand::EndGrab()
{
    ToggleActorPhysics(GrabbedActor, true);
    GrabbedActor = nullptr;
}

void ACraftHand::Tick(float DeltaSeconds)
{
    if (GrabbedActor)
    {
        GrabbedActor = OverlappingActor;

        FVector CurrentLoc = GrabbedActor->GetActorLocation();
        FVector TargetLoc = this->GetActorLocation();
        TargetLoc.Z = ObjectZPLane;

        // Smoothly interpolate
        FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaSeconds, 20.f); // 5.f = speed
        GrabbedActor->SetActorLocation(NewLoc);
    }
}

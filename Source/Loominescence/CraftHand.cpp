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
    UE_LOG(LogTemp, Warning, TEXT("Toggle physics"));
    
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

    UE_LOG(LogTemp, Warning, TEXT("=== COMPONENT TREE FOR %s ==="), *OverlappingActor->GetName());

    TArray<UActorComponent*> Components;
    OverlappingActor->GetComponents(Components);

    for (UActorComponent* C : Components)
    {
        if (!C) continue;

        UE_LOG(LogTemp, Warning, TEXT("Component: %s (%s)"),
            *C->GetName(),
            *C->GetClass()->GetName());
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
        Ingredient->CleanupOnDestroy(false);
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
        FVector CurrentLoc = GrabbedActor->GetActorLocation();
        FVector TargetLoc = this->GetActorLocation();
        TargetLoc.Z = ObjectZPLane;

        // Smoothly interpolate
        FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaSeconds, 20.f); // 5.f = speed
        UE_LOG(LogTemp, Warning, TEXT("Tick Grab: %s from %s -> %s"),
            *GrabbedActor->GetName(),
            *CurrentLoc.ToString(),
            *NewLoc.ToString());

        bool bMoved = GrabbedActor->SetActorLocation(NewLoc, false, nullptr, ETeleportType::None);
        if (UPrimitiveComponent* Prim = GrabbedActor->FindComponentByClass<UPrimitiveComponent>())
        {
            Prim->SetWorldLocation(NewLoc);
        }

        UE_LOG(LogTemp, Warning, TEXT("SetActorLocation returned: %s"),
            bMoved ? TEXT("true") : TEXT("false"));

        // Also dump root + primitive component location:
        if (USceneComponent* Root = GrabbedActor->GetRootComponent())
        {
            UE_LOG(LogTemp, Warning, TEXT("  Root(%s) at %s"),
                *Root->GetClass()->GetName(),
                *Root->GetComponentLocation().ToString());
        }

        if (UPrimitiveComponent* Prim = GrabbedActor->FindComponentByClass<UPrimitiveComponent>())
        {
            UE_LOG(LogTemp, Warning, TEXT("  Prim(%s) at %s | SimPhys=%d"),
                *Prim->GetClass()->GetName(),
                *Prim->GetComponentLocation().ToString(),
                Prim->IsSimulatingPhysics());

            if (Prim->GetAttachParent())
            {
                UE_LOG(LogTemp, Warning, TEXT("Mesh parent: %s (%s)"),
                    *Prim->GetAttachParent()->GetName(),
                    *Prim->GetAttachParent()->GetClass()->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Mesh has NO parent!"));
            }
        }
    }
}

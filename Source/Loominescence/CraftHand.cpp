// CraftHand.cpp
#include "CraftHand.h"

#include "CauldronActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

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

void ACraftHand::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Overlapping with: %s (Class: %s)"),
        *OtherActor->GetName(),
        *OtherActor->GetClass()->GetName());


    // If grabbing an object already --> do not execute overlap code
    if (GrabbedActor) return;
    
    if (OtherActor && OtherActor != this)
    {
        OverlappingActor = OtherActor;
        
        UE_LOG(LogTemp, Log, TEXT("Hand overlapping with: %s"), *OtherActor->GetName());

        ChangeHighlightActor( OverlappingActor, true);
    }
}

void ACraftHand::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    
    if (GrabbedActor) return;
    
    OverlappingActor = nullptr;
    ChangeHighlightActor(OverlappingActor, false);
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

    GrabbedActor = OverlappingActor;
    ToggleActorPhysics(GrabbedActor, false);
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
        FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaSeconds, 5.f); // 5.f = speed
        GrabbedActor->SetActorLocation(NewLoc);
    }
}

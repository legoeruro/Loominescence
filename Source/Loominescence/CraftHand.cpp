// CraftHand.cpp
#include "CraftHand.h"
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

void ChangeHighlightActor(const AActor* GrabbedActor, bool IsHighlighted)
{
    if (!GrabbedActor) return;
    UPrimitiveComponent* MeshComp = Cast<UPrimitiveComponent>(GrabbedActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));
    if (MeshComp)
    {
        MeshComp->SetRenderCustomDepth(IsHighlighted); // enables outline
    }
}

void ACraftHand::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Log, TEXT("Hand overlapping with: %s"), *OtherActor->GetName());
    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlapped"));


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
    
    if (OtherActor && OtherActor != this)
    {
        OverlappingActor = nullptr;
        
        UE_LOG(LogTemp, Log, TEXT("Hand stopped overlapping with: %s"), *OtherActor->GetName());

        ChangeHighlightActor(OverlappingActor, false);
    }
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

        // Get current location
        FVector CurrentLoc = GrabbedActor->GetActorLocation();
        FVector TargetLoc = this->GetActorLocation();
        TargetLoc.Z = ObjectZPLane; // lift to z = 2 meters, Unreal units = cm

        // Smoothly interpolate
        FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaSeconds, 5.f); // 5.f = speed
        GrabbedActor->SetActorLocation(NewLoc);
    }
}

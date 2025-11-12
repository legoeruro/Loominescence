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

void ACraftHand::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Log, TEXT("Hand overlapping with: %s"), *OtherActor->GetName());

    if (GrabbedActor) return;
    
    if (OtherActor && OtherActor != this)
    {
        OverlappingActor = OtherActor;
        
        UE_LOG(LogTemp, Log, TEXT("Hand overlapping with: %s"), *OtherActor->GetName());
        if(GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Collide"));

        //TODO: highlight the overlapped actor
    }
}

void ACraftHand::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this)
    {
        OverlappingActor = nullptr;
        
        UE_LOG(LogTemp, Log, TEXT("Hand stopped overlapping with: %s"), *OtherActor->GetName());
    }
}

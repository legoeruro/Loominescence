#include "SpawnerBoxActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

ASpawnerBoxActor::ASpawnerBoxActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	RootComponent = BoxMesh;

	BoxMesh->SetGenerateOverlapEvents(true);
	BoxMesh->SetCollisionProfileName("BlockAllDynamic");
	UE_LOG(LogTemp, Warning, TEXT("Initialize box actor"));
	UE_LOG(LogTemp, Warning, TEXT("Actor: %s"), *GetName());
}

void ASpawnerBoxActor::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInterface* BaseMat = BoxMesh->GetMaterial(0);
	if (!BaseMat)
	{
		UE_LOG(LogTemp, Error, TEXT("No material found on Mesh(0) for actor: %s"), *GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Base Material on %s = %s"), *GetName(), *BaseMat->GetName());

	// Create dynamic material
	if (BoxMesh && BoxMesh->GetMaterial(0))
	{
		UMaterialInstanceDynamic* WaterMaterial = UMaterialInstanceDynamic::Create(BoxMesh->GetMaterial(0), this);
		BoxMesh->SetMaterial(0, WaterMaterial);

		WaterMaterial->SetVectorParameterValue("TintOverlay", TintColor);
		UE_LOG(LogTemp, Warning, TEXT("Dynamic Material Created: %s"), *WaterMaterial->GetName());
	}
}


void ASpawnerBoxActor::TriggerSpawn()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawning ingredient"));
	SpawnAndLaunch();
}

void ASpawnerBoxActor::SpawnAndLaunch()
{
	if (!SpawnClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 100);
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AIngredientActor* NewObj = World->SpawnActor<AIngredientActor>(SpawnClass, SpawnLoc, FRotator::ZeroRotator, Params);
	
	if (NewObj)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned actor successfully"));

		FVector LaunchDir = FVector(FMath::FRandRange(-0.3f, 0.3f), FMath::FRandRange(-0.3f, 0.3f), 1.0f);
		LaunchDir.Normalize();

		if (UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(NewObj->GetRootComponent()))
		{
			Comp->SetSimulatePhysics(true);
			Comp->AddImpulse(LaunchDir * EjectForce, NAME_None, true);
		}
	}
}

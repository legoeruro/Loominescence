#include "SpawnerBoxActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Utils/LoomiUtils.h"
#include "GameFramework/Character.h"

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

	// Retrieve the inventory actor component
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (PlayerChar)
	{
		InventoryComp = PlayerChar->GetComponentByClass(UActorComponent::StaticClass());

		for (UActorComponent* Comp : PlayerChar->GetComponents())
		{
			// Assumes the BPC's name is Inventory
			if (Comp && Comp->GetName().Contains(TEXT("Inventory")))
			{
				InventoryComp = Comp;
				break;
			}
		}
	}

	UpdateBoxAvailability();
}

void ASpawnerBoxActor::UpdateBoxAvailability()
{
	int32 outIndex;
	bool bHasItem = LoomiUtils::FindItemInInventory(InventoryComp, ItemNameToSpawn, outIndex);
	bIsBoxUnavailable = !bHasItem;

	// If no item was found
	if (!bHasItem)
	{
		BoxMaterial->SetVectorParameterValue("TintOverlay", FLinearColor::Gray);
	}
	else
	{
		BoxMaterial->SetVectorParameterValue("TintOverlay", TintColor);
	}
}

void ASpawnerBoxActor::TriggerSpawn()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawning ingredient"));

	if (bIsBoxUnavailable)
		return;
		
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

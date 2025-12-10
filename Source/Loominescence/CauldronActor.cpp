#include "CauldronActor.h"
#include "Utils/GameTypes.h"
#include "IngredientActor.h"
#include "Utils/MixingManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Utils/ULoomiUtils.h"
#include "Kismet/GameplayStatics.h"

ACauldronActor::ACauldronActor()
{
    PrimaryActorTick.bCanEverTick = false;

    CauldronMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CauldronMesh"));
    RootComponent = CauldronMesh;

    WaterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterMesh"));
    WaterMesh->SetupAttachment(CauldronMesh);

    DropZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DropZone"));
    DropZone->SetupAttachment(CauldronMesh);
    DropZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DropZone->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void ACauldronActor::BeginPlay()
{
    Super::BeginPlay();

    // Bind overlap
    DropZone->OnComponentBeginOverlap.AddDynamic(this, &ACauldronActor::OnDropZoneOverlap);

    // Create dynamic material
    if (WaterMesh && WaterMesh->GetMaterial(0))
    {
        WaterMaterial = UMaterialInstanceDynamic::Create(WaterMesh->GetMaterial(0), this);
        WaterMesh->SetMaterial(0, WaterMaterial);

        UE_LOG(LogTemp, Warning, TEXT("Dynamic Material Created: %s"), *WaterMaterial->GetName());
    }
}

void ACauldronActor::OnDropZoneOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                       bool bFromSweep, const FHitResult& SweepResult)
{
    // Limit ingredients amount to 2
    if (CurrentIngredients.Num() >= 2)
    {
        EjectAgent(OtherActor);
        return;
    }
    
    AIngredientActor* Ingredient = Cast<AIngredientActor>(OtherActor);
    if (Ingredient)
    {
        StoredIngredientClasses.Add(Ingredient->GetClass());

        CurrentIngredients.Add(Ingredient);
        UpdateWaterColor();

        Ingredient->Destroy();

        UE_LOG(LogTemp, Log, TEXT("Ingredient %s added and destroyed"), *Ingredient->GetName());
    }
}

void ACauldronActor::UpdateWaterColor()
{
    if (CurrentIngredients.Num() == 0)
    {
        WaterMaterial->SetVectorParameterValue("BaseColor", FLinearColor::White);
        return;
    }
    if (!WaterMaterial || CurrentIngredients.Num() == 0) return;

    FLinearColor AvgColor = FLinearColor::Black;
    for (AIngredientActor* Ingr : CurrentIngredients)
    {
        AvgColor += Ingr->LiquidColor;
    }
    AvgColor /= CurrentIngredients.Num();

    UE_LOG(LogTemp, Warning, TEXT("Updating Water Color: R=%.2f G=%.2f B=%.2f Ingredients=%d"),
        AvgColor.R, AvgColor.G, AvgColor.B, CurrentIngredients.Num());

    WaterMaterial->SetVectorParameterValue("BaseColor", AvgColor);
}

void ACauldronActor::OnInteract()
{
    if (CurrentIngredients.Num() >= 2)
    {
        MixIngredients();
    }
    else if (CurrentIngredients.Num() == 1)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        UE_LOG(LogTemp, Warning, TEXT("ejecting ingredient"));
        
        // Get last ingredient class
        TSubclassOf<AIngredientActor> IngredientClass = StoredIngredientClasses.Last();
        StoredIngredientClasses.Pop();

        FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 100);
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        
        CurrentIngredients.Empty();
        UpdateWaterColor();
    }
}

FString ACauldronActor::GetStringListOfIngredients() const
{
    FString Result = TEXT("");

    if (CurrentIngredients.Num() == 0)
    {
        return TEXT("Nothingness");
    }

    for (int i = 0; i < CurrentIngredients.Num(); ++i)
    {
        AIngredientActor* Ingredient = CurrentIngredients[i];
        if (!Ingredient) continue;

        FString Name = Ingredient->IngredientName.ToString();
        FString ColorHex = Ingredient->LiquidColor.ToFColorSRGB().ToHex();

        Result += FString::Printf(
            TEXT("%s"),
            *Name
        );

        if (i != CurrentIngredients.Num() - 1)
            Result += TEXT(", ");
    }

    return Result;
}

void ACauldronActor::AddUnlockRecipeToJournal_Implementation(FMixRecipe Recipe)
{
    UE_LOG(LogTemp, Warning, TEXT("Impolement this in blueprints!"));
}

void ACauldronActor::EjectAgent(AActor* ThisActor)
{
    if (!ThisActor) return;

    ThisActor->SetActorHiddenInGame(false);
    ThisActor->SetActorEnableCollision(true);

    FVector LaunchDir = FVector(FMath::FRandRange(-0.3f, 0.3f), FMath::FRandRange(-0.3f, 0.3f), 1.0f);
    LaunchDir.Normalize();

    ThisActor->SetActorLocation(GetActorLocation() + FVector(0,0,200));
    if (UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(ThisActor->GetRootComponent()))
    {
        Comp->SetSimulatePhysics(true);
        Comp->AddImpulse(LaunchDir * EjectForce, NAME_None, true);
    }
}

void ACauldronActor::MixIngredients()
{
    if (CurrentIngredients.Num() < 2)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough ingredients to mix!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    EElementalType ElemA = CurrentIngredients[0]->ElementalType;
    EElementalType ElemB = CurrentIngredients[1]->ElementalType;

    // Destroy all ingredient actors
    for (AIngredientActor* Ingr : CurrentIngredients)
    {
        if (Ingr) Ingr->Destroy();
    }
    CurrentIngredients.Empty();

    const auto* MixingManager = ULoomiUtils::GetMixingManager(this);
    if (!MixingManager)
    {
        UE_LOG(LogTemp, Error, TEXT("MixingManager not found!"));
        return;
    }

    FPotionData PotionData = MixingManager->GetResult(ElemA, ElemB);
    FMixRecipe Recipe = MixingManager->GetRecipe(ElemB, ElemA);
    AddUnlockRecipeToJournal(Recipe);

    // Ensure we have a potion blueprint
    if (!PotionBlueprint)
    {
        UE_LOG(LogTemp, Error, TEXT("No PotionBlueprint assigned in Cauldron!"));
        return;
    }

    // Spawn potion
    FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 200);
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    APotionActor* SpawnedPotion = World->SpawnActor<APotionActor>(PotionBlueprint, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

    if (SpawnedPotion)
    {
        SpawnedPotion->InitializePotion(PotionData);

        // Launch potion
        if (UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(SpawnedPotion->GetRootComponent()))
        {
            Comp->SetSimulatePhysics(true);
            Comp->AddImpulse(FVector(100, 100, 600.f), NAME_None, true);
        }

        UE_LOG(LogTemp, Log, TEXT("Potion created and launched: %s"), *SpawnedPotion->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn potion, check blueprint class assignment"));
    }

    if (WaterMaterial)
        WaterMaterial->SetVectorParameterValue("BaseColor", FLinearColor::Transparent);
}
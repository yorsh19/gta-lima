#include "Procedural/LimaBuildingGenerator.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ALimaBuildingGenerator::ALimaBuildingGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SceneRoot->SetMobility(EComponentMobility::Static);
    RootComponent = SceneRoot;
    auto Mesh = [](const TCHAR* Folder, const TCHAR* Name) -> UStaticMesh*
    {
        const FString Path = FString::Printf(TEXT("/Game/KalexUrban/Environment/Buildings/%s/%s"), Folder, Name);
        ConstructorHelpers::FObjectFinderOptional<UStaticMesh> Finder(*Path);
        return Finder.Get();
    };
    Assets.FacadeWall050 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Wall_050"));
    Assets.FacadeWall100 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Wall_100"));
    Assets.FacadeWall150 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Wall_150"));
    Assets.FacadeWall200 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Wall_200"));
    Assets.FacadeWindow100 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Window_100"));
    Assets.FacadeWindow120 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Window_120"));
    Assets.FacadeWindow150 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Window_150"));
    Assets.FacadeDoor090 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Door_090"));
    Assets.FacadeDoor100 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Door_100"));
    Assets.FacadeGarage250 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Garage_250"));
    Assets.FacadeGarage300 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Facade_Garage_300"));
    Assets.Column025 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Column_025"));
    Assets.Beam025 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Beam_025"));
    Assets.RebarGroup01 = Mesh(TEXT("Modular"), TEXT("SM_SJL_Rebar_Group_01"));
    Assets.RoofFlat01 = Mesh(TEXT("Roofs"), TEXT("SM_SJL_Roof_Flat_01"));
    Assets.RoofUnfinished01 = Mesh(TEXT("Roofs"), TEXT("SM_SJL_Roof_Unfinished_01"));
    Assets.WaterTank01 = Mesh(TEXT("Props"), TEXT("SM_SJL_WaterTank_01"));
    Assets.BrickStack01 = Mesh(TEXT("Props"), TEXT("SM_SJL_BrickStack_01"));

    const TCHAR* Paths[] = {
        TEXT("Brick/M_SJL_Brick_Red"), TEXT("Brick/M_SJL_Brick_Dark"),
        TEXT("Plaster/M_SJL_Plaster"), TEXT("Paint/M_SJL_Paint_Blue"),
        TEXT("Paint/M_SJL_Paint_Orange"), TEXT("Paint/M_SJL_Paint_Green"),
        TEXT("Paint/M_SJL_Paint_Yellow"), TEXT("Tile/M_SJL_Tile_White"),
        TEXT("Tile/M_SJL_Tile_Blue"), TEXT("Tile/M_SJL_Tile_Beige")
    };
    for (const TCHAR* Path : Paths)
    {
        const FString FullPath = FString(TEXT("/Game/KalexUrban/Materials/Instances/")) + Path;
        ConstructorHelpers::FObjectFinderOptional<UMaterialInterface> Finder(*FullPath);
        FacadeMaterials.Add(Finder.Get());
    }
}

void ALimaBuildingGenerator::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    if (bGenerateOnConstruction) GenerateBuilding();
}

void ALimaBuildingGenerator::ClearBuilding()
{
    TArray<UStaticMeshComponent*> Components;
    GetComponents(Components);
    for (UStaticMeshComponent* Component : Components)
    {
        if (Component && Component->ComponentHasTag(TEXT("Generated")))
        {
            RemoveInstanceComponent(Component);
            Component->DestroyComponent();
        }
    }
    GeneratedComponents.Empty();
}

void ALimaBuildingGenerator::RandomizeBuilding()
{
    FRandomStream Random(Seed);
    Floors = Random.RandRange(1, 4);
    BuildingWidth = Random.RandRange(10, 18) * 50.f;
    BuildingDepth = Random.RandRange(16, 30) * 50.f;
    WindowsPerFloor = Random.RandRange(1, 3);
    bHasGarage = Random.FRand() < .55f;
    bHasStorefront = Random.FRand() < .20f;
    bUnfinishedRoof = Random.FRand() < .65f;
    bHasWaterTank = Random.FRand() < .45f;
    bHasBrickStack = Random.FRand() < .30f;
    FacadeType = static_cast<ELimaFacadeType>(Random.RandRange(0, 7));
    GenerateBuilding();
}

void ALimaBuildingGenerator::GenerateBuilding()
{
    ClearBuilding();

    // V5.2.2: optional foundation. Uses the engine cube so no new project asset is required.
    // The actor origin remains the finished floor level; the foundation only grows downward.
    if (bGenerateFoundation && FoundationDepth > 1.0f)
    {
        UStaticMesh* FoundationMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
        if (FoundationMesh)
        {
            const float TotalDepth = FoundationDepth + FoundationEmbed;
            UStaticMeshComponent* Foundation = NewObject<UStaticMeshComponent>(this);
            Foundation->SetupAttachment(SceneRoot);
            Foundation->SetStaticMesh(FoundationMesh);
            Foundation->SetRelativeLocation(FVector(0.f, 0.f, -TotalDepth * 0.5f + FoundationEmbed));
            Foundation->SetRelativeScale3D(FVector(BuildingWidth / 100.f, BuildingDepth / 100.f, TotalDepth / 100.f));
            Foundation->SetMobility(EComponentMobility::Static);
            Foundation->ComponentTags.Add(TEXT("Generated"));
            Foundation->SetCollisionProfileName(TEXT("BlockAll"));
            AddInstanceComponent(Foundation);
            Foundation->RegisterComponent();
            GeneratedComponents.Add(Foundation);
        }
    }
    // Also migrates roots serialized by older Blueprint instances.
    SceneRoot->SetMobility(EComponentMobility::Static);
    BuildingWidth = FMath::Clamp(BuildingWidth, 300.f, 2000.f);
    BuildingDepth = FMath::Clamp(BuildingDepth, 400.f, 3000.f);
    Floors = FMath::Clamp(Floors, 1, 6);
    FloorHeight = FMath::Clamp(FloorHeight, 220.f, 400.f);
    if (!Assets.FacadeWall050 || !Assets.FacadeWall100 || !Assets.FacadeWall150 ||
        !Assets.FacadeWall200 || !Assets.FacadeWindow100 || !Assets.FacadeWindow150 ||
        !Assets.FacadeDoor100 || !Assets.FacadeGarage250 || !Assets.FacadeGarage300)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: V3 facade modules missing. Run generate_assets.ps1 and import_to_unreal.ps1."), *GetName());
        return;
    }
    GenerateStructure();
    GenerateGroundFloor();
    for (int32 Floor = 1; Floor < Floors; ++Floor) GenerateUpperFloor(Floor);
    GenerateRoof();
}

UMaterialInterface* ALimaBuildingGenerator::GetFloorMaterial(int32 FloorIndex) const
{
    if (FloorMaterialOverrides.IsValidIndex(FloorIndex) && FloorMaterialOverrides[FloorIndex])
        return FloorMaterialOverrides[FloorIndex];
    FRandomStream Random(Seed);
    int32 Choice = 0;
    for (int32 Floor = 0; Floor <= FloorIndex; ++Floor) Choice = Random.RandRange(0, 9);
    const ELimaFacadeType Finish = FloorIndex == 0 && bOverrideGroundFloorFinish ? GroundFloorFinish : FacadeType;
    if ((!bVaryFinishBySeed || (FloorIndex == 0 && bOverrideGroundFloorFinish)) && Finish != ELimaFacadeType::Mixed)
    {
        switch (Finish)
        {
        case ELimaFacadeType::Brick: Choice = Random.RandRange(0,1); break;
        case ELimaFacadeType::Plaster: Choice = 2; break;
        case ELimaFacadeType::PaintedBlue: Choice = 3; break;
        case ELimaFacadeType::PaintedOrange: Choice = 4; break;
        case ELimaFacadeType::PaintedGreen: Choice = 5; break;
        case ELimaFacadeType::PaintedYellow: Choice = 6; break;
        case ELimaFacadeType::Tile: Choice = Random.RandRange(7,9); break;
        default: break;
        }
    }
    return FacadeMaterials.IsValidIndex(Choice) ? FacadeMaterials[Choice].Get() : nullptr;
}

void ALimaBuildingGenerator::GenerateStructure()
{
    // Corner columns are inset behind the facade and cannot obscure its openings.
    for (int32 Floor = 0; Floor < Floors; ++Floor)
        for (int32 X : {-1, 1})
            for (int32 Y : {-1, 1})
                AddMesh(Assets.Column025,
                    FVector(X*(BuildingWidth/2-22.5f), Y*(BuildingDepth/2-22.5f), Floor*FloorHeight),
                    FRotator::ZeroRotator, FVector(1,1,FloorHeight/280.f),
                    FString::Printf(TEXT("Column_%d_%d_%d"),Floor,X,Y));
}

void ALimaBuildingGenerator::GenerateGroundFloor()
{
    GenerateFacade(0);
    GenerateSideWalls(0);
    GenerateBackWall(0);
}

void ALimaBuildingGenerator::GenerateUpperFloor(int32 FloorIndex)
{
    GenerateFacade(FloorIndex);
    GenerateSideWalls(FloorIndex);
    GenerateBackWall(FloorIndex);
}

void ALimaBuildingGenerator::GenerateFacade(int32 FloorIndex)
{
    // Compose on a 50cm grid; distribute only the sub-grid residual across the run.
    const int32 Slots = FMath::Max(6, FMath::RoundToInt(BuildingWidth/50.f));
    const float Correction = BuildingWidth/(Slots*50.f);
    const float Y = -BuildingDepth/2;
    const float Z = FloorIndex*FloorHeight;
    float Cursor = -BuildingWidth/2;
    int32 Part = 0;
    auto Opening = [&](UStaticMesh* Mesh, int32 Units)
    {
        const float Width = Units*50.f*Correction;
        AddMesh(Mesh, FVector(Cursor+Width/2,Y,Z), FRotator::ZeroRotator,
            FVector(Correction,1,FloorHeight/280.f),
            FString::Printf(TEXT("Opening_%d_%d"),FloorIndex,Part++), GetFloorMaterial(FloorIndex));
        Cursor += Width;
    };
    auto Wall = [&](int32 Units)
    {
        if (Units <= 0) return;
        const float Width = Units*50.f*Correction;
        AddWallRun(FVector(Cursor,Y,Z), FRotator::ZeroRotator, Width, FloorIndex,
            FString::Printf(TEXT("Front_%d_%d"),FloorIndex,Part++));
        Cursor += Width;
    };
    FRandomStream Random(Seed);
    for (int32 I = 0; I <= FloorIndex; ++I) Random.GetUnsignedInt();
    if (FloorIndex == 0)
    {
        int32 GarageUnits = 0;
        if ((bHasGarage || bHasStorefront) && Slots >= 9)
            GarageUnits = Slots >= 10 && Random.RandRange(0,1) ? 6 : 5;
        const int32 WallUnits = Slots - 2 - GarageUnits;
        const int32 Left = FMath::Max(1, WallUnits/3);
        Wall(Left);
        if (GarageUnits) Opening(GarageUnits == 6 ? Assets.FacadeGarage300 : Assets.FacadeGarage250, GarageUnits);
        Wall(GarageUnits ? 1 : 0);
        Opening(Assets.FacadeDoor100, 2);
        Wall(WallUnits-Left-(GarageUnits ? 1 : 0));
    }
    else
    {
        const int32 WindowUnits = Random.RandRange(2,3);
        const int32 Count = FMath::Clamp(WindowsPerFloor, 0, FMath::Max(0,(Slots-1)/(WindowUnits+1)));
        const int32 Walls = Slots-Count*WindowUnits;
        for (int32 I = 0; I <= Count; ++I)
        {
            Wall(Walls/(Count+1)+(I < Walls%(Count+1) ? 1 : 0));
            if (I < Count) Opening(WindowUnits == 3 ? Assets.FacadeWindow150 : Assets.FacadeWindow100,WindowUnits);
        }
    }
}

void ALimaBuildingGenerator::AddWallRun(const FVector& Start, const FRotator& Rotation,
    float Width, int32 FloorIndex, const FString& Name)
{
    const int32 Units = FMath::Max(1,FMath::RoundToInt(Width/50.f));
    const float Correction = Width/(Units*50.f);
    int32 Remaining = Units;
    float Cursor = 0;
    int32 Index = 0;
    while (Remaining > 0)
    {
        const int32 Size = FMath::Min(Remaining,4);
        UStaticMesh* Mesh = Size == 4 ? Assets.FacadeWall200.Get() : Size == 3 ?
            Assets.FacadeWall150.Get() : Size == 2 ? Assets.FacadeWall100.Get() : Assets.FacadeWall050.Get();
        const float Span = Size*50.f*Correction;
        AddMesh(Mesh, Start+Rotation.RotateVector(FVector(Cursor+Span/2,0,0)),Rotation,
            FVector(Correction,1,FloorHeight/280.f),FString::Printf(TEXT("%s_%d"),*Name,Index++),
            GetFloorMaterial(FloorIndex));
        Cursor += Span;
        Remaining -= Size;
    }
}

void ALimaBuildingGenerator::GenerateSideWalls(int32 FloorIndex)
{
    // Stop at inner faces of front/back modules, avoiding coplanar corner overlap.
    for (int32 Side : {-1,1})
        AddWallRun(FVector(Side*(BuildingWidth/2-10.f),-BuildingDepth/2+10.f,FloorIndex*FloorHeight),
            FRotator(0,90,0),BuildingDepth-20.f,FloorIndex,
            FString::Printf(TEXT("Side_%d_%d"),FloorIndex,Side));
}

void ALimaBuildingGenerator::GenerateBackWall(int32 FloorIndex)
{
    AddWallRun(FVector(-BuildingWidth/2,BuildingDepth/2,FloorIndex*FloorHeight),
        FRotator::ZeroRotator,BuildingWidth,FloorIndex,FString::Printf(TEXT("Back_%d"),FloorIndex));
}

void ALimaBuildingGenerator::GenerateRoof()
{
    const float Z = Floors*FloorHeight;
    // Flat slab only: exposed rebars remain unscaled independent pieces.
    AddMesh(Assets.RoofFlat01,FVector(0,0,Z),FRotator::ZeroRotator,
        FVector(BuildingWidth/600.f,BuildingDepth/1200.f,1),TEXT("Roof"));
    FRandomStream Random(Seed);
    if (bUnfinishedRoof)
    {
        for (int32 X : {-1,1})
            for (int32 Y : {-1,1})
                if (Random.FRand() < .8f)
                    AddMesh(Assets.RebarGroup01,FVector(X*(BuildingWidth/2-22.5f),Y*(BuildingDepth/2-22.5f),Z+20),
                        FRotator::ZeroRotator,FVector::OneVector,FString::Printf(TEXT("Rebar_%d_%d"),X,Y));
    }
    else
    {
        // A low parapet from the existing 1m beam module.
        const int32 Count = FMath::Max(1,FMath::RoundToInt(BuildingWidth/100.f));
        const float Span = BuildingWidth/Count;
        for (int32 I=0; I<Count; ++I)
            AddMesh(Assets.Beam025,FVector(-BuildingWidth/2+(I+.5f)*Span,-BuildingDepth/2,Z+20),
                FRotator::ZeroRotator,FVector(Span/100.f,1,1),FString::Printf(TEXT("Parapet_%d"),I));
    }
    if (bHasWaterTank)
        AddMesh(Assets.WaterTank01,FVector(BuildingWidth*.25f,BuildingDepth*.15f,Z+20),
            FRotator::ZeroRotator,FVector::OneVector,TEXT("WaterTank"));
    if (bHasBrickStack)
        AddMesh(Assets.BrickStack01,FVector(-BuildingWidth*.25f,BuildingDepth*.20f,Z+20),
            FRotator::ZeroRotator,FVector::OneVector,TEXT("BrickStack"));
}

void ALimaBuildingGenerator::AddMesh(UStaticMesh* Mesh, const FVector& Location,
    const FRotator& Rotation, const FVector& Scale, const FString& ComponentName,
    UMaterialInterface* Finish)
{
    if (!Mesh) return;
    UStaticMeshComponent* Component = NewObject<UStaticMeshComponent>(this,
        MakeUniqueObjectName(this,UStaticMeshComponent::StaticClass(),*ComponentName));
    Component->SetupAttachment(SceneRoot);
    Component->SetMobility(EComponentMobility::Static);
    Component->SetStaticMesh(Mesh);
    Component->SetRelativeTransform(FTransform(Rotation,Location,Scale));
    if (Finish)
    {
        for (int32 I=0; I<Mesh->GetStaticMaterials().Num(); ++I)
        {
            const FString Slot = Mesh->GetStaticMaterials()[I].MaterialSlotName.ToString();
            if (Slot == TEXT("M_SJL_Brick_Red"))
                Component->SetMaterial(I,Finish);
        }
    }
    Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Component->SetCollisionResponseToAllChannels(ECR_Block);
    Component->ComponentTags.Add(TEXT("Generated"));
    Component->RegisterComponent();
    AddInstanceComponent(Component);
    GeneratedComponents.Add(Component);
}

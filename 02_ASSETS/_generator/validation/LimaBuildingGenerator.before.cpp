#include "Procedural/LimaBuildingGenerator.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"


ALimaBuildingGenerator::ALimaBuildingGenerator()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(
        TEXT("Root")
    );

    RootComponent = SceneRoot;
}


void ALimaBuildingGenerator::OnConstruction(
    const FTransform& Transform
)
{
    Super::OnConstruction(Transform);

    if (bGenerateOnConstruction)
    {
        GenerateBuilding();
    }
}


// ============================================================
// CLEAR
// ============================================================

void ALimaBuildingGenerator::ClearBuilding()
{
    TArray<UStaticMeshComponent*> MeshComponents;

    GetComponents<UStaticMeshComponent>(
        MeshComponents
    );

    for (UStaticMeshComponent* Component : MeshComponents)
    {
        if (
            Component &&
            Component->ComponentHasTag(TEXT("Generated"))
        )
        {
            Component->DestroyComponent();
        }
    }

    GeneratedComponents.Empty();
}


// ============================================================
// RANDOMIZE
// ============================================================

void ALimaBuildingGenerator::RandomizeBuilding()
{
    FRandomStream Random(Seed);

    Floors = Random.RandRange(
        1,
        4
    );

    BuildingWidth = Random.FRandRange(
        500.0f,
        900.0f
    );

    BuildingDepth = Random.FRandRange(
        800.0f,
        1500.0f
    );

    WindowsPerFloor = Random.RandRange(
        1,
        3
    );

    bHasGarage =
        Random.FRand() < 0.55f;

    bHasStorefront =
        Random.FRand() < 0.20f;

    bUnfinishedRoof =
        Random.FRand() < 0.65f;

    bHasWaterTank =
        Random.FRand() < 0.45f;

    bHasBrickStack =
        Random.FRand() < 0.30f;

    const int32 FacadeIndex =
        Random.RandRange(0, 3);

    FacadeType =
        static_cast<ELimaFacadeType>(
            FacadeIndex
        );

    GenerateBuilding();
}


// ============================================================
// GENERATE
// ============================================================

void ALimaBuildingGenerator::GenerateBuilding()
{
    ClearBuilding();

    GenerateStructure();

    GenerateGroundFloor();

    for (int32 Floor = 1; Floor < Floors; ++Floor)
    {
        GenerateUpperFloor(Floor);
    }

    GenerateRoof();
}


// ============================================================
// WALL SELECTION
// ============================================================

UStaticMesh* ALimaBuildingGenerator::GetWallMesh() const
{
    switch (FacadeType)
    {
        case ELimaFacadeType::Plaster:
            return Assets.WallPlaster300;

        case ELimaFacadeType::PaintedBlue:
            return Assets.WallBlue300;

        case ELimaFacadeType::PaintedOrange:
            return Assets.WallOrange300;

        case ELimaFacadeType::Brick:
        default:
            return Assets.WallBrick300;
    }
}


// ============================================================
// STRUCTURE
// ============================================================

void ALimaBuildingGenerator::GenerateStructure()
{
    /*
     * For MVP the modular facade itself represents most of
     * the shell. Structural columns/slabs will be added in
     * the next asset pass.
     */
}


// ============================================================
// GROUND FLOOR
// ============================================================

void ALimaBuildingGenerator::GenerateGroundFloor()
{
    const float Z = FloorHeight * 0.5f;

    const float FrontY =
        -(BuildingDepth * 0.5f);

    const float DoorWidth = 100.0f;

    const float GarageWidth =
        bHasGarage ? 280.0f : 0.0f;

    // --------------------------------------------------------
    // GARAGE
    // --------------------------------------------------------

    if (
        bHasGarage &&
        Assets.GarageDoor01
    )
    {
        const float GarageX =
            -(BuildingWidth * 0.5f)
            + GarageWidth * 0.5f;

        AddMesh(
            Assets.GarageDoor01,
            FVector(
                GarageX,
                FrontY - 2.0f,
                0.0f
            ),
            FRotator::ZeroRotator,
            FVector::OneVector,
            TEXT("GarageDoor")
        );
    }

    // --------------------------------------------------------
    // DOOR
    // --------------------------------------------------------

    if (Assets.DoorMetal01)
    {
        const float DoorX =
            (BuildingWidth * 0.5f)
            - DoorWidth * 0.5f;

        AddMesh(
            Assets.DoorMetal01,
            FVector(
                DoorX,
                FrontY - 3.0f,
                0.0f
            ),
            FRotator::ZeroRotator,
            FVector::OneVector,
            TEXT("Door")
        );
    }

    // --------------------------------------------------------
    // WALL AREA
    // --------------------------------------------------------

    const float UsedWidth =
        GarageWidth +
        DoorWidth;

    const float Remaining =
        FMath::Max(
            100.0f,
            BuildingWidth - UsedWidth
        );

    const float WallX =
        -BuildingWidth * 0.5f +
        GarageWidth +
        Remaining * 0.5f;

    AddWallSegment(
        FVector(
            WallX,
            FrontY,
            Z
        ),
        FRotator::ZeroRotator,
        Remaining,
        TEXT("GroundFrontWall")
    );

    GenerateSideWalls(0);
    GenerateBackWall(0);
}


// ============================================================
// UPPER FLOORS
// ============================================================

void ALimaBuildingGenerator::GenerateUpperFloor(
    int32 FloorIndex
)
{
    const float BaseZ =
        FloorIndex * FloorHeight;

    const float CenterZ =
        BaseZ + FloorHeight * 0.5f;

    const float FrontY =
        -(BuildingDepth * 0.5f);

    AddWallSegment(
        FVector(
            0.0f,
            FrontY,
            CenterZ
        ),
        FRotator::ZeroRotator,
        BuildingWidth,
        FString::Printf(
            TEXT("FrontWall_%d"),
            FloorIndex
        )
    );

    // --------------------------------------------------------
    // WINDOWS
    // --------------------------------------------------------

    if (
        Assets.WindowGrille01 &&
        WindowsPerFloor > 0
    )
    {
        const float Margin = 100.0f;

        const float AvailableWidth =
            BuildingWidth - Margin * 2.0f;

        const float Spacing =
            AvailableWidth /
            static_cast<float>(
                WindowsPerFloor + 1
            );

        for (
            int32 WindowIndex = 0;
            WindowIndex < WindowsPerFloor;
            ++WindowIndex
        )
        {
            const float X =
                -BuildingWidth * 0.5f
                + Margin
                + Spacing *
                    static_cast<float>(
                        WindowIndex + 1
                    );

            AddMesh(
                Assets.WindowGrille01,
                FVector(
                    X,
                    FrontY - 12.0f,
                    BaseZ + 145.0f
                ),
                FRotator::ZeroRotator,
                FVector::OneVector,
                FString::Printf(
                    TEXT("Window_%d_%d"),
                    FloorIndex,
                    WindowIndex
                )
            );
        }
    }

    GenerateSideWalls(
        FloorIndex
    );

    GenerateBackWall(
        FloorIndex
    );
}


// ============================================================
// SIDE WALLS
// ============================================================

void ALimaBuildingGenerator::GenerateSideWalls(
    int32 FloorIndex
)
{
    const float Z =
        FloorIndex * FloorHeight +
        FloorHeight * 0.5f;

    const float HalfWidth =
        BuildingWidth * 0.5f;

    AddWallSegment(
        FVector(
            -HalfWidth,
            0.0f,
            Z
        ),
        FRotator(
            0.0f,
            90.0f,
            0.0f
        ),
        BuildingDepth,
        FString::Printf(
            TEXT("LeftWall_%d"),
            FloorIndex
        )
    );

    AddWallSegment(
        FVector(
            HalfWidth,
            0.0f,
            Z
        ),
        FRotator(
            0.0f,
            90.0f,
            0.0f
        ),
        BuildingDepth,
        FString::Printf(
            TEXT("RightWall_%d"),
            FloorIndex
        )
    );
}


// ============================================================
// BACK
// ============================================================

void ALimaBuildingGenerator::GenerateBackWall(
    int32 FloorIndex
)
{
    const float Z =
        FloorIndex * FloorHeight +
        FloorHeight * 0.5f;

    AddWallSegment(
        FVector(
            0.0f,
            BuildingDepth * 0.5f,
            Z
        ),
        FRotator(
            0.0f,
            180.0f,
            0.0f
        ),
        BuildingWidth,
        FString::Printf(
            TEXT("BackWall_%d"),
            FloorIndex
        )
    );
}


// ============================================================
// ROOF
// ============================================================

void ALimaBuildingGenerator::GenerateRoof()
{
    UStaticMesh* RoofMesh =
        bUnfinishedRoof
            ? Assets.RoofUnfinished01
            : Assets.RoofFlat01;

    const float RoofZ =
        Floors * FloorHeight;

    if (RoofMesh)
    {
        /*
         * Generated roof source mesh = 600 x 1200 cm.
         */
        const FVector RoofScale(
            BuildingWidth / 600.0f,
            BuildingDepth / 1200.0f,
            1.0f
        );

        AddMesh(
            RoofMesh,
            FVector(
                0.0f,
                0.0f,
                RoofZ
            ),
            FRotator::ZeroRotator,
            RoofScale,
            TEXT("Roof")
        );
    }

    if (
        bHasWaterTank &&
        Assets.WaterTank01
    )
    {
        AddMesh(
            Assets.WaterTank01,
            FVector(
                BuildingWidth * 0.25f,
                BuildingDepth * 0.15f,
                RoofZ + 10.0f
            ),
            FRotator::ZeroRotator,
            FVector::OneVector,
            TEXT("WaterTank")
        );
    }

    if (
        bHasBrickStack &&
        Assets.BrickStack01
    )
    {
        AddMesh(
            Assets.BrickStack01,
            FVector(
                -BuildingWidth * 0.25f,
                BuildingDepth * 0.20f,
                RoofZ + 10.0f
            ),
            FRotator::ZeroRotator,
            FVector::OneVector,
            TEXT("BrickStack")
        );
    }
}


// ============================================================
// WALL HELPER
// ============================================================

void ALimaBuildingGenerator::AddWallSegment(
    const FVector& Location,
    const FRotator& Rotation,
    float DesiredWidth,
    const FString& Name
)
{
    UStaticMesh* Mesh =
        GetWallMesh();

    if (!Mesh)
    {
        return;
    }

    /*
     * Base modular wall = 300 cm wide.
     */
    constexpr float BaseWidth =
        300.0f;

    constexpr float BaseHeight =
        280.0f;

    FVector Scale(
        DesiredWidth / BaseWidth,
        1.0f,
        FloorHeight / BaseHeight
    );

    AddMesh(
        Mesh,
        Location,
        Rotation,
        Scale,
        Name
    );
}


// ============================================================
// COMPONENT CREATION
// ============================================================

void ALimaBuildingGenerator::AddMesh(
    UStaticMesh* Mesh,
    const FVector& Location,
    const FRotator& Rotation,
    const FVector& Scale,
    const FString& ComponentName
)
{
    if (!Mesh)
    {
        return;
    }

    UStaticMeshComponent* Component =
        NewObject<UStaticMeshComponent>(
            this,
            *ComponentName
        );

    if (!Component)
    {
        return;
    }

    Component->SetupAttachment(
        SceneRoot
    );

    Component->SetStaticMesh(
        Mesh
    );

    Component->SetRelativeLocation(
        Location
    );

    Component->SetRelativeRotation(
        Rotation
    );

    Component->SetRelativeScale3D(
        Scale
    );

    Component->SetMobility(
        EComponentMobility::Static
    );

    Component->SetCollisionEnabled(
        ECollisionEnabled::QueryAndPhysics
    );

    Component->SetCollisionResponseToAllChannels(
        ECR_Block
    );

    Component->ComponentTags.Add(
        TEXT("Generated")
    );

    Component->RegisterComponent();

    AddInstanceComponent(
        Component
    );

    GeneratedComponents.Add(
        Component
    );
}
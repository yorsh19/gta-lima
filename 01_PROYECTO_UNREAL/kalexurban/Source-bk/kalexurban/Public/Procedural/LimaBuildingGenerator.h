#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "LimaBuildingGenerator.generated.h"


class USceneComponent;
class UStaticMesh;
class UMaterialInterface;
class UStaticMeshComponent;
class UHierarchicalInstancedStaticMeshComponent;


UENUM(BlueprintType)
enum class ELimaFacadeType : uint8
{
    Brick       UMETA(DisplayName = "Exposed Brick"),
    Plaster     UMETA(DisplayName = "Plaster"),
    PaintedBlue UMETA(DisplayName = "Painted Blue"),
    PaintedOrange UMETA(DisplayName = "Painted Orange"),
    Tile UMETA(DisplayName = "Tile"),
    Mixed UMETA(DisplayName = "Mixed"),
    PaintedGreen UMETA(DisplayName = "Painted Green"),
    PaintedYellow UMETA(DisplayName = "Painted Yellow")
};


USTRUCT(BlueprintType)
struct FLimaBuildingAssets
{
    GENERATED_BODY()

    // V3: geometry is shared by every finish; legacy asset fields below remain serialized.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeWall050 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeWall100 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeWall150 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeWall200 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeWindow100 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeWindow120 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeWindow150 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeDoor090 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeDoor100 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeGarage250 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> FacadeGarage300 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> Column025 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> Beam025 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> RebarGroup01 = nullptr;


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> WallBrick300 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> WallBrick400 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> WallPlaster300 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> WallBlue300 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> WallOrange300 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> DoorMetal01 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> GarageDoor01 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> WindowGrille01 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> RoofFlat01 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> RoofUnfinished01 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> WaterTank01 = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> BrickStack01 = nullptr;
};


UCLASS(Blueprintable)
class KALEXURBAN_API ALimaBuildingGenerator : public AActor
{
    GENERATED_BODY()

public:

    ALimaBuildingGenerator();

    virtual void OnConstruction(
        const FTransform& Transform
    ) override;


    // ========================================================
    // GENERATION
    // ========================================================

    UFUNCTION(
        CallInEditor,
        BlueprintCallable,
        Category = "Kalex Urban|Generation"
    )
    void GenerateBuilding();


    UFUNCTION(
        CallInEditor,
        BlueprintCallable,
        Category = "Kalex Urban|Generation"
    )
    void RandomizeBuilding();


    UFUNCTION(
        CallInEditor,
        BlueprintCallable,
        Category = "Kalex Urban|Generation"
    )
    void ClearBuilding();


    // ========================================================
    // KALEX CITY V5.2 - EDITOR OWNERSHIP
    // ========================================================

    /** If enabled, GenerateCity/ClearGeneratedCity will never delete this actor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex Urban|Generation")
    bool bManualOverride = false;

    /** Locks a generated building in place while keeping its procedural parameters editable. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex Urban|Generation")
    bool bLockFromRegeneration = false;

    /** Stable source id (lot id / manual-zone candidate id) for editor identification. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kalex Urban|Generation")
    FString GenerationId;

    // ========================================================
    // KALEX CITY V5.2.2 - TERRAIN FOUNDATION (ADDITIVE)
    // ========================================================

    /** Generates a simple structural plinth below the house when the lot has slope. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex Urban|Terrain")
    bool bGenerateFoundation = false;

    /** Foundation depth below actor origin, in Unreal centimeters. Set by LimaCityGenerator. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex Urban|Terrain", meta=(ClampMin="0.0", ClampMax="5000.0"))
    float FoundationDepth = 0.0f;

    /** Small extra depth so the foundation penetrates the Landscape and never visually floats. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex Urban|Terrain", meta=(ClampMin="0.0", ClampMax="100.0"))
    float FoundationEmbed = 10.0f;

    // ========================================================
    // RANDOMIZATION
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Random"
    )
    int32 Seed = 12345;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Random"
    )
    bool bGenerateOnConstruction = true;


    // ========================================================
    // DIMENSIONS
    // Unreal units = centimeters
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Dimensions",
        meta = (ClampMin = "300.0", ClampMax = "2000.0")
    )
    float BuildingWidth = 600.0f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Dimensions",
        meta = (ClampMin = "400.0", ClampMax = "3000.0")
    )
    float BuildingDepth = 1000.0f;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Dimensions",
        meta = (ClampMin = "1", ClampMax = "6")
    )
    int32 Floors = 2;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Dimensions",
        meta = (ClampMin = "220.0", ClampMax = "400.0")
    )
    float FloorHeight = 280.0f;


    // ========================================================
    // APPEARANCE
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Appearance"
    )
    ELimaFacadeType FacadeType = ELimaFacadeType::Brick;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Appearance"
    )
    bool bHasGarage = true;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Appearance"
    )
    bool bHasStorefront = false;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Appearance"
    )
    bool bUnfinishedRoof = true;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Appearance"
    )
    bool bHasWaterTank = true;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Appearance"
    )
    bool bHasBrickStack = false;


    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Appearance",
        meta = (ClampMin = "0", ClampMax = "5")
    )
    int32 WindowsPerFloor = 2;


    // ========================================================
    // ASSETS
    // ========================================================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Kalex Urban|Assets"
    )
    FLimaBuildingAssets Assets;


    // Disable to select FacadeType explicitly; per-floor overrides always take precedence.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex Urban|Appearance")
    bool bVaryFinishBySeed = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex Urban|Appearance")
    bool bOverrideGroundFloorFinish = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex Urban|Appearance")
    ELimaFacadeType GroundFloorFinish = ELimaFacadeType::Tile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex Urban|Materials")
    TArray<TObjectPtr<UMaterialInterface>> FloorMaterialOverrides;

    // Brick red/dark, plaster, blue/orange/green/yellow, tile white/blue/beige.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex Urban|Materials")
    TArray<TObjectPtr<UMaterialInterface>> FacadeMaterials;

protected:

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> SceneRoot;


private:

    UMaterialInterface* GetFloorMaterial(int32 FloorIndex) const;
    void GenerateFacade(int32 FloorIndex);
    void AddWallRun(const FVector& Start, const FRotator& Rotation, float Width,
                    int32 FloorIndex, const FString& Name);

    void GenerateStructure();
    void GenerateGroundFloor();
    void GenerateUpperFloor(int32 FloorIndex);
    void GenerateRoof();

    void GenerateSideWalls(
        int32 FloorIndex
    );

    void GenerateBackWall(
        int32 FloorIndex
    );

    void AddMesh(
        UStaticMesh* Mesh,
        const FVector& Location,
        const FRotator& Rotation,
        const FVector& Scale,
        const FString& ComponentName,
        UMaterialInterface* Finish = nullptr
    );

    UPROPERTY(Transient)
    TArray<TObjectPtr<UStaticMeshComponent>>
        GeneratedComponents;
};
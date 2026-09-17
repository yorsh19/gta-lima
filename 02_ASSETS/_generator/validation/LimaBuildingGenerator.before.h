#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "LimaBuildingGenerator.generated.h"


class USceneComponent;
class UStaticMesh;
class UStaticMeshComponent;
class UHierarchicalInstancedStaticMeshComponent;


UENUM(BlueprintType)
enum class ELimaFacadeType : uint8
{
    Brick       UMETA(DisplayName = "Exposed Brick"),
    Plaster     UMETA(DisplayName = "Plaster"),
    PaintedBlue UMETA(DisplayName = "Painted Blue"),
    PaintedOrange UMETA(DisplayName = "Painted Orange")
};


USTRUCT(BlueprintType)
struct FLimaBuildingAssets
{
    GENERATED_BODY()

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


protected:

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> SceneRoot;


private:

    UStaticMesh* GetWallMesh() const;

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
        const FString& ComponentName
    );

    void AddWallSegment(
        const FVector& Location,
        const FRotator& Rotation,
        float DesiredWidth,
        const FString& Name
    );

    TArray<TObjectPtr<UStaticMeshComponent>>
        GeneratedComponents;
};
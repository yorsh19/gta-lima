#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "City/LimaCityData.h"

#include "LimaCityGenerator.generated.h"

class USceneComponent;
class USplineComponent;
class USplineMeshComponent;
class UStaticMesh;
class AKalexUrbanZone;
class APlayerStart;

UCLASS()
class KALEXURBAN_API ALimaCityGenerator : public AActor
{
    GENERATED_BODY()

public:

    ALimaCityGenerator();

protected:

    virtual void OnConstruction(
        const FTransform& Transform
    ) override;

    virtual void BeginPlay() override;

public:

    /*
     * ============================================================
     * ROOT
     * ============================================================
     */

    UPROPERTY(
        VisibleAnywhere,
        Category = "Kalex Urban"
    )
    TObjectPtr<USceneComponent> SceneRoot;


    /*
     * ============================================================
     * CITY
     * ============================================================
     */

    /**
     * Ruta hacia city.json generado por Python.
     *
     * Puede ser:
     *
     * - absoluta
     * - relativa al directorio del proyecto Unreal
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|City"
    )
    FString CityJsonPath =
        TEXT(
            "../../../03_CITY_DATA/"
            "SJL_CantoGrande_MVP/"
            "processed/city.json"
        );


    /**
     * Unreal trabaja normalmente en centímetros.
     *
     * 1 metro = 100 Unreal Units.
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|City",
        meta = (
            ClampMin = "1.0",
            UIMin = "1.0"
        )
    )
    float UnitsPerMeter = 100.0f;


    /*
     * ============================================================
     * ROADS
     * ============================================================
     */

    /**
     * Static Mesh utilizado para construir cada segmento vial.
     *
     * Ejemplo:
     *
     * SM_SJL_Road_Asphalt_2Lane
     *
     * IMPORTANTE:
     * El eje longitudinal del mesh debe ser X.
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|Roads"
    )
    TObjectPtr<UStaticMesh> RoadMesh = nullptr;


    /**
     * Pequeño offset vertical para evitar Z-fighting.
     *
     * 5 UU = 5 cm.
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|Roads"
    )
    float RoadZOffset = 5.0f;


    /**
     * Ancho usado cuando OSM no proporciona width.
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|Roads",
        meta = (
            ClampMin = "1.0",
            UIMin = "1.0"
        )
    )
    float DefaultRoadWidthMeters = 7.0f;


    /**
     * Ancho REAL que representa RoadMesh cuando su Scale Y = 1.
     *
     * Si SM_SJL_Road_Asphalt_2Lane mide 8 metros de ancho:
     *
     * RoadMeshBaseWidthMeters = 8
     *
     * Entonces una calle OSM de 6 m utilizará:
     *
     * ScaleY = 6 / 8 = 0.75
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|Roads",
        meta = (
            ClampMin = "0.1",
            UIMin = "0.1"
        )
    )
    float RoadMeshBaseWidthMeters = 8.0f;


    /**
     * Generar geometría física de carreteras.
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|Roads"
    )
    bool bGenerateRoadMeshes = true;


    /**
     * Activa colisión en los meshes viales.
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|Roads"
    )
    bool bRoadCollision = true;


    /*
     * ============================================================
     * DEBUG
     * ============================================================
     */

    /**
     * Mostrar líneas de las carreteras.
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|Debug"
    )
    bool bDrawRoadDebug = false;


    /**
     * Mostrar footprints OSM de edificios.
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|Debug"
    )
    bool bDrawBuildingFootprints = true;


    /**
     * Mostrar polígonos de parques.
     */
    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|Debug"
    )
    bool bDrawParks = true;


    UPROPERTY(
        EditAnywhere,
        Category = "Kalex Urban|Debug"
    )
    float DebugLineThickness = 15.0f;


    /*
     * ============================================================
     * STATS
     * ============================================================
     */

    UPROPERTY(
        VisibleAnywhere,
        Category = "Kalex Urban|Stats"
    )
    int32 LoadedRoads = 0;


    UPROPERTY(VisibleAnywhere, Category = "Kalex Urban|Stats")
    int32 LoadedBlocks = 0;

    UPROPERTY(VisibleAnywhere, Category = "Kalex Urban|Stats")
    int32 LoadedLots = 0;

    UPROPERTY(
        VisibleAnywhere,
        Category = "Kalex Urban|Stats"
    )
    int32 LoadedBuildings = 0;


    UPROPERTY(
        VisibleAnywhere,
        Category = "Kalex Urban|Stats"
    )
    int32 LoadedParks = 0;


    UPROPERTY(
        VisibleAnywhere,
        Category = "Kalex Urban|Stats"
    )
    int32 GeneratedRoadSplines = 0;


    UPROPERTY(
        VisibleAnywhere,
        Category = "Kalex Urban|Stats"
    )
    int32 GeneratedRoadSegments = 0;


    /*
     * ============================================================
     * EDITOR ACTIONS
     * ============================================================
     */

    /**
     * Únicamente carga city.json.
     */
    UFUNCTION(
        CallInEditor,
        Category = "Kalex Urban"
    )
    void LoadCity();


    /**
     * Carga city.json y genera:
     *
     * - carreteras físicas
     * - debug de carreteras
     * - footprints de edificios
     * - parques
     */
    UFUNCTION(
        CallInEditor,
        Category = "Kalex Urban"
    )
    void GeneratePreview();


    /**
     * Elimina todo lo generado por este actor.
     */
    /** Full MVP generation: roads + procedural buildings from derived lots. */
    UFUNCTION(CallInEditor, Category = "Kalex Urban")
    void GenerateCity();

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings")
    bool bGenerateProceduralBuildings = true;

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings", meta=(ClampMin="0", ClampMax="2000"))
    int32 MaxProceduralBuildings = 2000;

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings", meta=(ClampMin="0.5", ClampMax="1.0"))
    float BuildingLotCoverage = 0.88f;

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement")
    bool bSnapBuildingsToTerrain = true;

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement", meta=(ClampMin="0.0"))
    float BuildingRoadSetbackMeters = 0.15f;

    /** Extra safety margin around the physical road corridor. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement", meta=(ClampMin="0.0", ClampMax="5.0"))
    float RoadSafetyMarginMeters = 0.20f;

    /** Reject candidates whose complete oriented footprint does not fit inside the source lot. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement")
    bool bRequireBuildingInsideLot = true;

    /** Prevent generated houses from overlapping other generated/manual locked houses. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement")
    bool bPreventBuildingOverlaps = true;

    /** If a lot touches a road corridor, progressively shrink the building instead of discarding the lot. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement")
    bool bAdaptiveRoadFit = true;

    /** Minimum lot coverage allowed while trying to fit a house between real road corridors. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement", meta=(ClampMin="0.45", ClampMax="1.0"))
    float MinAdaptiveBuildingCoverage = 0.62f;

    /** Coverage decrement used by the adaptive fitter. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement", meta=(ClampMin="0.01", ClampMax="0.20"))
    float AdaptiveCoverageStep = 0.04f;

    /** Pedestrian-only OSM ways do not reserve a full vehicular road corridor. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement")
    bool bIgnorePedestrianWaysForBuildings = true;

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement", meta=(ClampMin="1000.0"))
    float GroundTraceHeight = 100000.0f;

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement", meta=(ClampMin="1000.0"))
    float GroundTraceDepth = 100000.0f;

    // V5.2.2 terrain adaptation. Disabled road snapping by default so V5.2.1 road placement is preserved.
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain")
    bool bAdaptBuildingsToTerrain = true;

    /** Sample the four footprint corners and place the house at the highest ground point. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain")
    bool bSampleBuildingFootprintGround = true;

    /** Generate a downward foundation when corner height difference exceeds this value (meters). */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain", meta=(ClampMin="0.0", ClampMax="2.0"))
    float FoundationMinSlopeMeters = 0.20f;

    /** Reject extreme lots rather than creating absurd foundations. 0 disables rejection. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain", meta=(ClampMin="0.0", ClampMax="20.0"))
    float MaxBuildingGroundDeltaMeters = 4.0f;

    /** Optional V5.2.2 road-to-landscape adaptation. Keep false until building terrain pass is validated. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain")
    bool bSnapRoadsToTerrain = false;

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain", meta=(ClampMin="0.0", ClampMax="1.0"))
    float RoadTerrainOffsetMeters = 0.03f;

    /** V5.3.1 SAFE: when terrain snapping is enabled, resample roads densely in XY before tracing Z.
     *  This avoids long spline segments cutting through hills or floating over valleys. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain")
    bool bDenseRoadTerrainSampling = false;

    /** Distance between terrain samples along the OSM centerline. 2m is a safe MVP default. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain", meta=(ClampMin="0.5", ClampMax="10.0"))
    float RoadTerrainSampleSpacingMeters = 2.0f;

    /** V5.3.3: stable terrain-road mode. Keeps OSM XY immutable, samples only Landscape Z,
     *  and smooths the longitudinal profile before building spline meshes. New property so
     *  old serialized bSnapRoadsToTerrain values cannot silently disable the fix. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain|Stable Roads")
    bool bUseStableTerrainRoads = true;

    /** Half-window, in samples, used to smooth road elevation. 2 = five-sample moving average. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain|Stable Roads", meta=(ClampMin="0", ClampMax="8"))
    int32 RoadElevationSmoothRadius = 2;

    /** Extra vertical clearance above sampled Landscape to avoid z-fighting. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Terrain|Stable Roads", meta=(ClampMin="0.01", ClampMax="0.20"))
    float StableRoadSurfaceOffsetMeters = 0.05f;

    /** Hard minimum clearance between a generated building footprint and the physical road edge. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement", meta=(ClampMin="0.0", ClampMax="5.0"))
    float MinimumPhysicalRoadClearanceMeters = 0.75f;

    /** V5.3.3: additional no-building buffer measured from the asphalt edge. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Buildings|Placement", meta=(ClampMin="0.0", ClampMax="5.0"))
    float StableRoadBuildingBufferMeters = 1.25f;

    /** MVP spawn correction. Does not move PlayerStart or city coordinates; it places the pawn
     *  at the selected PlayerStart after BeginPlay. If PreferredPlayerStart is null this is only
     *  applied when the level contains exactly one PlayerStart. */
    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Spawn")
    bool bForceExactPlayerStartOnPlay = true;

    UPROPERTY(EditInstanceOnly, Category = "Kalex Urban|Spawn")
    TObjectPtr<APlayerStart> PreferredPlayerStart = nullptr;

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Zones")
    bool bGenerateManualZones = true;

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Debug")
    bool bDrawBlocks = false;

    UPROPERTY(EditAnywhere, Category = "Kalex Urban|Debug")
    bool bDrawLots = false;

    UPROPERTY(VisibleAnywhere, Category = "Kalex Urban|Stats")
    int32 GeneratedBuildings = 0;

    UFUNCTION(
        CallInEditor,
        Category = "Kalex Urban"
    )
    void ClearGeneratedCity();


private:

    /*
     * ============================================================
     * DATA
     * ============================================================
     */

    FLimaCityData CityData;


    /*
     * ============================================================
     * LOADING
     * ============================================================
     */

    bool LoadCityInternal();

    bool LoadCityFromJson(
        const FString& FilePath
    );

    FString ResolveJsonPath() const;


    /*
     * ============================================================
     * GENERATION
     * ============================================================
     */

    enum class EBuildingRejectReason : uint8
    {
        None,
        Road,
        Park,
        Ground,
        OutsideLot,
        BuildingOverlap,
        Spawn
    };

    void GenerateRoadMeshes();
    void GenerateBuildings();
    void GenerateManualZones();
    void GenerateZone(AKalexUrbanZone* Zone);
    bool SnapLocationToGround(FVector& WorldLocation) const;
    bool SampleBuildingGround(const TArray<FVector2D>& LocalFootprint, float& OutMinZ, float& OutMaxZ, float& OutAverageZ) const;
    bool IsBuildingClearOfRoads(const FVector2D& LocalCenter, float HalfWidthUU, float HalfDepthUU, float YawDeg, float SetbackMeters) const;
    bool IsPointInsidePark(const FVector2D& Point) const;
    TArray<FVector2D> MakeBuildingFootprint(const FVector2D& Center, float HalfWidthUU, float HalfDepthUU, float YawDeg) const;
    bool IsFootprintInsidePolygon(const TArray<FVector2D>& Footprint, const TArray<FVector2D>& Polygon) const;
    bool DoesFootprintOverlapGenerated(const TArray<FVector2D>& Footprint) const;
    void CacheLockedManualBuildings();
    bool SpawnProceduralBuilding(const FVector2D& LocalCenter, float WidthMeters, float DepthMeters, float YawDeg, int32 Floors, bool bCommercial, bool bUnfinished, int32 Seed, float Coverage, float RoadSetbackMeters, bool bSnapGround, EBuildingRejectReason* OutRejectReason = nullptr, const TArray<FVector2D>* AllowedLotFootprint = nullptr, const FString& GenerationId = FString());

    TArray<TArray<FVector2D>> OccupiedBuildingFootprints;

    void DrawRoadDebug();
    void DrawBlocks();
    void DrawLots();

    void DrawBuildingFootprints();

    void DrawParks();


    /*
     * ============================================================
     * HELPERS
     * ============================================================
     */

    float GetRoadWidthMeters(
        const FLimaRoadData& Road
    ) const;

    FColor GetRoadDebugColor(
        const FLimaRoadData& Road
    ) const;
};
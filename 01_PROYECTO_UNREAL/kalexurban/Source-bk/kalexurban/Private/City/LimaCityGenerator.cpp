#include "City/LimaCityGenerator.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Procedural/LimaBuildingGenerator.h"
#include "City/KalexUrbanZone.h"
#include "Components/BoxComponent.h"
#include "LandscapeProxy.h"

#include "DrawDebugHelpers.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"


/*
 * ================================================================
 * CONSTRUCTOR
 * ================================================================
 */

ALimaCityGenerator::ALimaCityGenerator()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("SceneRoot")
        );

    RootComponent = SceneRoot;
}


/*
 * ================================================================
 * CONSTRUCTION
 * ================================================================
 */

void ALimaCityGenerator::OnConstruction(
    const FTransform& Transform
)
{
    Super::OnConstruction(Transform);

    /*
     * IMPORTANTE:
     *
     * No generamos la ciudad automáticamente aquí.
     *
     * Si lo hiciéramos, mover el actor o modificar una propiedad
     * podría reconstruir cientos/miles de componentes.
     *
     * La generación se controla mediante GeneratePreview().
     */
}


/*
 * ================================================================
 * PATH
 * ================================================================
 */

FString ALimaCityGenerator::ResolveJsonPath() const
{
    if (FPaths::IsRelative(CityJsonPath))
    {
        return FPaths::ConvertRelativePathToFull(
            FPaths::ProjectDir(),
            CityJsonPath
        );
    }

    return CityJsonPath;
}


/*
 * ================================================================
 * PUBLIC LOAD BUTTON
 * ================================================================
 */

void ALimaCityGenerator::LoadCity()
{
    LoadCityInternal();
}


/*
 * ================================================================
 * INTERNAL LOAD
 * ================================================================
 */

bool ALimaCityGenerator::LoadCityInternal()
{
    const FString Path =
        ResolveJsonPath();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("KALEX CITY: Loading %s"),
        *Path
    );

    if (!LoadCityFromJson(Path))
    {
        LoadedRoads = 0;
        LoadedBuildings = 0;
        LoadedParks = 0;

        UE_LOG(
            LogTemp,
            Error,
            TEXT("KALEX CITY: FAILED")
        );

        return false;
    }

    LoadedRoads =
        CityData.Roads.Num();
    LoadedBlocks = CityData.Blocks.Num();
    LoadedLots = CityData.Lots.Num();

    LoadedBuildings =
        CityData.Buildings.Num();

    LoadedParks =
        CityData.Parks.Num();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "KALEX CITY LOADED | "
            "Roads=%d Blocks=%d Lots=%d Buildings=%d Parks=%d"
        ),
        LoadedRoads, LoadedBlocks, LoadedLots,
        LoadedBuildings, LoadedParks
    );

    return true;
}


/*
 * ================================================================
 * JSON
 * ================================================================
 */

bool ALimaCityGenerator::LoadCityFromJson(
    const FString& FilePath
)
{
    FString JsonString;

    if (!FFileHelper::LoadFileToString(
        JsonString,
        *FilePath
    ))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "KALEX CITY: Could not read city.json: %s"
            ),
            *FilePath
        );

        return false;
    }


    TSharedPtr<FJsonObject> Root;

    const TSharedRef<TJsonReader<>> Reader =
        TJsonReaderFactory<>::Create(
            JsonString
        );


    if (
        !FJsonSerializer::Deserialize(
            Reader,
            Root
        )
        ||
        !Root.IsValid()
    )
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("KALEX CITY: Invalid city JSON")
        );

        return false;
    }


    /*
     * Reset de los datos anteriores.
     */

    CityData = FLimaCityData();


    /*
     * ============================================================
     * GENERAL
     * ============================================================
     */

    Root->TryGetStringField(
        TEXT("name"),
        CityData.Name
    );


    /*
     * ============================================================
     * ORIGIN
     * ============================================================
     */

    const TSharedPtr<FJsonObject>* Origin;

    if (
        Root->TryGetObjectField(
            TEXT("origin"),
            Origin
        )
    )
    {
        (*Origin)->TryGetNumberField(
            TEXT("latitude"),
            CityData.OriginLatitude
        );

        (*Origin)->TryGetNumberField(
            TEXT("longitude"),
            CityData.OriginLongitude
        );
    }


    /*
     * ============================================================
     * ROADS
     * ============================================================
     */

    const TArray<TSharedPtr<FJsonValue>>* Roads;

    if (
        Root->TryGetArrayField(
            TEXT("roads"),
            Roads
        )
    )
    {
        for (const auto& Value : *Roads)
        {
            const TSharedPtr<FJsonObject> RoadObject =
                Value->AsObject();

            if (!RoadObject)
            {
                continue;
            }


            FLimaRoadData Road;

            double Number = 0.0;


            /*
             * ID
             */

            if (
                RoadObject->TryGetNumberField(
                    TEXT("id"),
                    Number
                )
            )
            {
                Road.Id =
                    static_cast<int64>(
                        Number
                    );
            }


            /*
             * NAME
             */

            RoadObject->TryGetStringField(
                TEXT("name"),
                Road.Name
            );


            /*
             * TYPE
             */

            RoadObject->TryGetStringField(
                TEXT("type"),
                Road.Type
            );


            /*
             * SURFACE
             */

            RoadObject->TryGetStringField(
                TEXT("surface"),
                Road.Surface
            );


            /*
             * LANES
             */

            if (
                RoadObject->TryGetNumberField(
                    TEXT("lanes"),
                    Number
                )
            )
            {
                Road.Lanes =
                    static_cast<int32>(
                        Number
                    );
            }


            /*
             * WIDTH
             */

            if (
                RoadObject->TryGetNumberField(
                    TEXT("width"),
                    Number
                )
            )
            {
                Road.Width =
                    static_cast<float>(
                        Number
                    );
            }


            /*
             * ONE WAY
             */

            RoadObject->TryGetBoolField(
                TEXT("oneway"),
                Road.bOneWay
            );


            /*
             * POINTS
             */

            const TArray<TSharedPtr<FJsonValue>>* Points;

            if (
                RoadObject->TryGetArrayField(
                    TEXT("points"),
                    Points
                )
            )
            {
                for (
                    const auto& PointValue :
                    *Points
                )
                {
                    const TArray<
                        TSharedPtr<FJsonValue>
                    >& Array =
                        PointValue->AsArray();

                    if (Array.Num() < 2)
                    {
                        continue;
                    }


                    const double X =
                        Array[0]->AsNumber();

                    const double Y =
                        Array[1]->AsNumber();

                    const double Z =
                        Array.Num() >= 3
                        ? Array[2]->AsNumber()
                        : 0.0;


                    Road.Points.Add(
                        FVector(
                            X * UnitsPerMeter,
                            Y * UnitsPerMeter,
                            Z * UnitsPerMeter
                        )
                    );
                }
            }


            if (Road.Points.Num() >= 2)
            {
                CityData.Roads.Add(
                    MoveTemp(Road)
                );
            }
        }
    }


    /*
     * ============================================================
     * BUILDINGS
     * ============================================================
     */

    const TArray<TSharedPtr<FJsonValue>>* Buildings;

    if (
        Root->TryGetArrayField(
            TEXT("buildings"),
            Buildings
        )
    )
    {
        for (const auto& Value : *Buildings)
        {
            const TSharedPtr<FJsonObject> Object =
                Value->AsObject();

            if (!Object)
            {
                continue;
            }


            FLimaBuildingData Building;

            double Number = 0.0;


            /*
             * ID
             */

            if (
                Object->TryGetNumberField(
                    TEXT("id"),
                    Number
                )
            )
            {
                Building.Id =
                    static_cast<int64>(
                        Number
                    );
            }


            /*
             * NAME
             */

            Object->TryGetStringField(
                TEXT("name"),
                Building.Name
            );


            /*
             * TYPE
             */

            Object->TryGetStringField(
                TEXT("type"),
                Building.Type
            );


            /*
             * FLOORS
             */

            if (
                Object->TryGetNumberField(
                    TEXT("floors"),
                    Number
                )
            )
            {
                Building.Floors =
                    static_cast<int32>(
                        Number
                    );
            }


            /*
             * HEIGHT
             */

            if (
                Object->TryGetNumberField(
                    TEXT("height"),
                    Number
                )
            )
            {
                Building.Height = Number;
            }


            /*
             * FOOTPRINT
             */

            const TArray<TSharedPtr<FJsonValue>>* Footprint;

            if (
                Object->TryGetArrayField(
                    TEXT("footprint"),
                    Footprint
                )
            )
            {
                for (
                    const auto& PointValue :
                    *Footprint
                )
                {
                    const TArray<
                        TSharedPtr<FJsonValue>
                    >& Point =
                        PointValue->AsArray();

                    if (Point.Num() < 2)
                    {
                        continue;
                    }


                    Building.Footprint.Add(
                        FVector2D(
                            Point[0]->AsNumber()
                                * UnitsPerMeter,

                            Point[1]->AsNumber()
                                * UnitsPerMeter
                        )
                    );
                }
            }


            if (
                Building.Footprint.Num()
                >= 3
            )
            {
                CityData.Buildings.Add(
                    MoveTemp(Building)
                );
            }
        }
    }


    /* ============================================================
     * BLOCKS + LOTS (derived by Python from the road graph)
     * ============================================================ */
    const TArray<TSharedPtr<FJsonValue>>* Blocks;
    if (Root->TryGetArrayField(TEXT("blocks"), Blocks))
    {
        for (const auto& Value : *Blocks)
        {
            const TSharedPtr<FJsonObject> O = Value->AsObject();
            if (!O) continue;
            FLimaBlockData Block; double N = 0.0;
            if (O->TryGetNumberField(TEXT("id"), N)) Block.Id = static_cast<int64>(N);
            if (O->TryGetNumberField(TEXT("area"), N)) Block.Area = static_cast<float>(N);
            const TArray<TSharedPtr<FJsonValue>>* Poly;
            if (O->TryGetArrayField(TEXT("polygon"), Poly))
                for (const auto& PV : *Poly)
                {
                    const auto& A = PV->AsArray();
                    if (A.Num() >= 2) Block.Polygon.Add(FVector2D(A[0]->AsNumber()*UnitsPerMeter, A[1]->AsNumber()*UnitsPerMeter));
                }
            if (Block.Polygon.Num() >= 3) CityData.Blocks.Add(MoveTemp(Block));
        }
    }

    const TArray<TSharedPtr<FJsonValue>>* Lots;
    if (Root->TryGetArrayField(TEXT("lots"), Lots))
    {
        for (const auto& Value : *Lots)
        {
            const TSharedPtr<FJsonObject> O = Value->AsObject();
            if (!O) continue;
            FLimaCityLotData Lot; double N = 0.0;
            if (O->TryGetNumberField(TEXT("id"), N)) Lot.Id = static_cast<int64>(N);
            if (O->TryGetNumberField(TEXT("blockId"), N)) Lot.BlockId = static_cast<int64>(N);
            if (O->TryGetNumberField(TEXT("width"), N)) Lot.Width = static_cast<float>(N);
            if (O->TryGetNumberField(TEXT("depth"), N)) Lot.Depth = static_cast<float>(N);
            if (O->TryGetNumberField(TEXT("yaw"), N)) Lot.Yaw = static_cast<float>(N);
            if (O->TryGetNumberField(TEXT("floors"), N)) Lot.Floors = static_cast<int32>(N);
            O->TryGetBoolField(TEXT("commercial"), Lot.bCommercial);
            O->TryGetBoolField(TEXT("unfinished"), Lot.bUnfinished);
            const TArray<TSharedPtr<FJsonValue>>* Center;
            if (O->TryGetArrayField(TEXT("center"), Center) && Center->Num() >= 2)
                Lot.Center = FVector2D((*Center)[0]->AsNumber()*UnitsPerMeter, (*Center)[1]->AsNumber()*UnitsPerMeter);
            const TArray<TSharedPtr<FJsonValue>>* Poly;
            if (O->TryGetArrayField(TEXT("footprint"), Poly))
                for (const auto& PV : *Poly)
                {
                    const auto& A = PV->AsArray();
                    if (A.Num() >= 2) Lot.Footprint.Add(FVector2D(A[0]->AsNumber()*UnitsPerMeter, A[1]->AsNumber()*UnitsPerMeter));
                }
            CityData.Lots.Add(MoveTemp(Lot));
        }
    }

    /*
     * ============================================================
     * PARKS
     * ============================================================
     */

    const TArray<TSharedPtr<FJsonValue>>* Parks;

    if (
        Root->TryGetArrayField(
            TEXT("parks"),
            Parks
        )
    )
    {
        for (const auto& Value : *Parks)
        {
            const TSharedPtr<FJsonObject> Object =
                Value->AsObject();

            if (!Object)
            {
                continue;
            }


            FLimaParkData Park;

            double Number = 0.0;


            /*
             * ID
             */

            if (
                Object->TryGetNumberField(
                    TEXT("id"),
                    Number
                )
            )
            {
                Park.Id =
                    static_cast<int64>(
                        Number
                    );
            }


            /*
             * NAME
             */

            Object->TryGetStringField(
                TEXT("name"),
                Park.Name
            );


            /*
             * TYPE
             */

            Object->TryGetStringField(
                TEXT("type"),
                Park.Type
            );


            /*
             * POLYGON
             */

            const TArray<TSharedPtr<FJsonValue>>* Polygon;

            if (
                Object->TryGetArrayField(
                    TEXT("polygon"),
                    Polygon
                )
            )
            {
                for (
                    const auto& PointValue :
                    *Polygon
                )
                {
                    const TArray<
                        TSharedPtr<FJsonValue>
                    >& Point =
                        PointValue->AsArray();

                    if (Point.Num() < 2)
                    {
                        continue;
                    }


                    Park.Polygon.Add(
                        FVector2D(
                            Point[0]->AsNumber()
                                * UnitsPerMeter,

                            Point[1]->AsNumber()
                                * UnitsPerMeter
                        )
                    );
                }
            }


            if (Park.Polygon.Num() >= 3)
            {
                CityData.Parks.Add(
                    MoveTemp(Park)
                );
            }
        }
    }


    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "KALEX CITY JSON PARSED | "
            "Roads=%d Blocks=%d Lots=%d Buildings=%d Parks=%d"
        ),
        CityData.Roads.Num(), CityData.Blocks.Num(), CityData.Lots.Num(),
        CityData.Buildings.Num(), CityData.Parks.Num()
    );


    return true;
}


/*
 * ================================================================
 * GENERATE PREVIEW
 * ================================================================
 */

void ALimaCityGenerator::GeneratePreview()
{
    /*
     * Primero eliminamos cualquier generación anterior.
     */

    ClearGeneratedCity();


    /*
     * Volvemos a cargar city.json.
     */

    if (!LoadCityInternal())
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "KALEX CITY: "
                "Preview cancelled because city.json failed"
            )
        );

        return;
    }


    /*
     * Geometría real.
     */

    if (bGenerateRoadMeshes)
    {
        GenerateRoadMeshes();
    }


    /*
     * Debug.
     */

    if (bDrawRoadDebug)
    {
        DrawRoadDebug();
    }

    if (bDrawBuildingFootprints)
    {
        DrawBuildingFootprints();
    }

    if (bDrawParks)
    {
        DrawParks();
    }


    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "KALEX CITY PREVIEW GENERATED | "
            "RoadSplines=%d RoadSegments=%d"
        ),
        GeneratedRoadSplines,
        GeneratedRoadSegments
    );
}


void ALimaCityGenerator::GenerateCity()
{
    ClearGeneratedCity();
    if (!LoadCityInternal()) return;
    if (bGenerateRoadMeshes) GenerateRoadMeshes();
    if (bGenerateProceduralBuildings) GenerateBuildings();
    if (bGenerateManualZones) GenerateManualZones();
    if (bDrawRoadDebug) DrawRoadDebug();
    if (bDrawBuildingFootprints) DrawBuildingFootprints();
    if (bDrawParks) DrawParks();
    if (bDrawBlocks) DrawBlocks();
    if (bDrawLots) DrawLots();
    UE_LOG(LogTemp, Warning, TEXT("KALEX CITY GENERATED | Roads=%d Segments=%d Buildings=%d Blocks=%d Lots=%d"),
        GeneratedRoadSplines, GeneratedRoadSegments, GeneratedBuildings, CityData.Blocks.Num(), CityData.Lots.Num());
}

void ALimaCityGenerator::GenerateBuildings()
{
    GeneratedBuildings = 0;
    OccupiedBuildingFootprints.Reset();
    CacheLockedManualBuildings();

    int32 RejectedTooSmall = 0;
    int32 RejectedRoad = 0;
    int32 RejectedPark = 0;
    int32 RejectedGround = 0;
    int32 RejectedOutsideLot = 0;
    int32 RejectedOverlap = 0;
    int32 SpawnFailed = 0;

    const int32 Count = MaxProceduralBuildings <= 0 ? CityData.Lots.Num() : FMath::Min(MaxProceduralBuildings, CityData.Lots.Num());

    for (int32 I = 0; I < Count; ++I)
    {
        const FLimaCityLotData& Lot = CityData.Lots[I];
        if (Lot.Width < 3.f || Lot.Depth < 4.f)
        {
            ++RejectedTooSmall;
            continue;
        }

        const int32 Seed = static_cast<int32>((Lot.Id * 2654435761ULL) & 0x7fffffff);
        EBuildingRejectReason RejectReason = EBuildingRejectReason::None;
        const TArray<FVector2D>* LotPolygon =
            (bRequireBuildingInsideLot && Lot.Footprint.Num() >= 3) ? &Lot.Footprint : nullptr;

        bool bSpawned = false;
        float TryCoverage = BuildingLotCoverage;
        do
        {
            RejectReason = EBuildingRejectReason::None;
            bSpawned = SpawnProceduralBuilding(
                Lot.Center, Lot.Width, Lot.Depth, Lot.Yaw,
                FMath::Clamp(Lot.Floors, 1, 6), Lot.bCommercial, Lot.bUnfinished,
                Seed, TryCoverage, BuildingRoadSetbackMeters,
                bSnapBuildingsToTerrain, &RejectReason, LotPolygon,
                FString::Printf(TEXT("LOT_%lld"), Lot.Id));

            // Only road/outside-lot failures can reasonably be solved by shrinking the footprint.
            if (bSpawned || !bAdaptiveRoadFit ||
                (RejectReason != EBuildingRejectReason::Road && RejectReason != EBuildingRejectReason::OutsideLot))
            {
                break;
            }

            TryCoverage -= AdaptiveCoverageStep;
        }
        while (TryCoverage + KINDA_SMALL_NUMBER >= MinAdaptiveBuildingCoverage);

        if (!bSpawned)
        {
            switch (RejectReason)
            {
                case EBuildingRejectReason::Road:            ++RejectedRoad; break;
                case EBuildingRejectReason::Park:            ++RejectedPark; break;
                case EBuildingRejectReason::Ground:          ++RejectedGround; break;
                case EBuildingRejectReason::OutsideLot:      ++RejectedOutsideLot; break;
                case EBuildingRejectReason::BuildingOverlap: ++RejectedOverlap; break;
                default:                                     ++SpawnFailed; break;
            }
        }
    }

    UE_LOG(LogTemp, Warning,
        TEXT("KALEX BUILDINGS V5.2.2 | Checked=%d Spawned=%d TooSmall=%d Road=%d Park=%d Ground=%d OutsideLot=%d Overlap=%d SpawnFailed=%d"),
        Count, GeneratedBuildings, RejectedTooSmall, RejectedRoad, RejectedPark,
        RejectedGround, RejectedOutsideLot, RejectedOverlap, SpawnFailed);
}

static float DistPointSegment2D(const FVector2D& P, const FVector2D& A, const FVector2D& B)
{
    const FVector2D AB = B - A;
    const float Den = AB.SizeSquared();
    if (Den <= KINDA_SMALL_NUMBER) return FVector2D::Distance(P, A);
    const float T = FMath::Clamp(FVector2D::DotProduct(P - A, AB) / Den, 0.f, 1.f);
    return FVector2D::Distance(P, A + AB * T);
}

static float Cross2D(const FVector2D& A, const FVector2D& B, const FVector2D& C)
{
    return (B.X-A.X)*(C.Y-A.Y) - (B.Y-A.Y)*(C.X-A.X);
}

static bool SegmentsIntersect2D(const FVector2D& A, const FVector2D& B, const FVector2D& C, const FVector2D& D)
{
    const float C1=Cross2D(A,B,C), C2=Cross2D(A,B,D), C3=Cross2D(C,D,A), C4=Cross2D(C,D,B);
    return ((C1 >= 0.f && C2 <= 0.f) || (C1 <= 0.f && C2 >= 0.f)) &&
           ((C3 >= 0.f && C4 <= 0.f) || (C3 <= 0.f && C4 >= 0.f));
}

static float DistSegmentSegment2D(const FVector2D& A, const FVector2D& B, const FVector2D& C, const FVector2D& D)
{
    if (SegmentsIntersect2D(A,B,C,D)) return 0.f;
    return FMath::Min(FMath::Min(DistPointSegment2D(A,C,D), DistPointSegment2D(B,C,D)),
                      FMath::Min(DistPointSegment2D(C,A,B), DistPointSegment2D(D,A,B)));
}

static bool PointInsidePolygon2D(const FVector2D& P, const TArray<FVector2D>& Poly)
{
    bool Inside=false;
    const int32 N=Poly.Num();
    if (N < 3) return false;
    for (int32 I=0,J=N-1; I<N; J=I++)
    {
        const FVector2D& A=Poly[I]; const FVector2D& B=Poly[J];
        if (((A.Y>P.Y)!=(B.Y>P.Y)) &&
            (P.X < (B.X-A.X)*(P.Y-A.Y)/(B.Y-A.Y+SMALL_NUMBER)+A.X)) Inside=!Inside;
    }
    return Inside;
}

TArray<FVector2D> ALimaCityGenerator::MakeBuildingFootprint(const FVector2D& C, float HalfW, float HalfD, float YawDeg) const
{
    const float R=FMath::DegreesToRadians(YawDeg), Co=FMath::Cos(R), Si=FMath::Sin(R);
    const FVector2D X(Co,Si), Y(-Si,Co);
    TArray<FVector2D> P;
    P.Reserve(4);
    P.Add(C-X*HalfW-Y*HalfD); P.Add(C+X*HalfW-Y*HalfD);
    P.Add(C+X*HalfW+Y*HalfD); P.Add(C-X*HalfW+Y*HalfD);
    return P;
}

bool ALimaCityGenerator::IsFootprintInsidePolygon(const TArray<FVector2D>& Footprint, const TArray<FVector2D>& Polygon) const
{
    if (Polygon.Num() < 3) return true;
    for (const FVector2D& P : Footprint)
        if (!PointInsidePolygon2D(P, Polygon)) return false;
    return true;
}

static bool PolygonsOverlap2D(const TArray<FVector2D>& A, const TArray<FVector2D>& B)
{
    if (A.Num()<3 || B.Num()<3) return false;
    for (int32 I=0; I<A.Num(); ++I)
        for (int32 J=0; J<B.Num(); ++J)
            if (SegmentsIntersect2D(A[I],A[(I+1)%A.Num()],B[J],B[(J+1)%B.Num()])) return true;
    return PointInsidePolygon2D(A[0],B) || PointInsidePolygon2D(B[0],A);
}

bool ALimaCityGenerator::DoesFootprintOverlapGenerated(const TArray<FVector2D>& Footprint) const
{
    if (!bPreventBuildingOverlaps) return false;
    for (const TArray<FVector2D>& Existing : OccupiedBuildingFootprints)
        if (PolygonsOverlap2D(Footprint, Existing)) return true;
    return false;
}

void ALimaCityGenerator::CacheLockedManualBuildings()
{
    UWorld* World=GetWorld(); if(!World) return;
    for (TActorIterator<ALimaBuildingGenerator> It(World); It; ++It)
    {
        ALimaBuildingGenerator* B=*It;
        if (!B || (!B->bManualOverride && !B->bLockFromRegeneration)) continue;
        const FVector Local3=GetActorTransform().InverseTransformPosition(B->GetActorLocation());
        const float LocalYaw=B->GetActorRotation().Yaw-GetActorRotation().Yaw;
        OccupiedBuildingFootprints.Add(MakeBuildingFootprint(FVector2D(Local3.X,Local3.Y), B->BuildingWidth*.5f, B->BuildingDepth*.5f, LocalYaw));
    }
}

bool ALimaCityGenerator::IsBuildingClearOfRoads(
    const FVector2D& C, float HalfW, float HalfD, float YawDeg, float SetbackMeters) const
{
    if (SetbackMeters < 0.f) return true;

    const TArray<FVector2D> Footprint=MakeBuildingFootprint(C,HalfW,HalfD,YawDeg);
    const float SetbackUU=(FMath::Max(0.f,SetbackMeters)+RoadSafetyMarginMeters)*UnitsPerMeter;

    for (const FLimaRoadData& Road : CityData.Roads)
    {
        if (Road.Points.Num()<2) continue;

        if (bIgnorePedestrianWaysForBuildings &&
            (Road.Type == TEXT("footway") || Road.Type == TEXT("path") ||
             Road.Type == TEXT("cycleway") || Road.Type == TEXT("steps") ||
             Road.Type == TEXT("pedestrian")))
        {
            continue;
        }
        const float Corridor=GetRoadWidthMeters(Road)*UnitsPerMeter*.5f+SetbackUU;
        for (int32 I=0; I<Road.Points.Num()-1; ++I)
        {
            const FVector2D A(Road.Points[I].X,Road.Points[I].Y), B(Road.Points[I+1].X,Road.Points[I+1].Y);
            if (PointInsidePolygon2D(A,Footprint) || PointInsidePolygon2D(B,Footprint)) return false;
            for (int32 E=0; E<4; ++E)
                if (DistSegmentSegment2D(A,B,Footprint[E],Footprint[(E+1)%4]) < Corridor) return false;
        }
    }
    return true;
}

bool ALimaCityGenerator::IsPointInsidePark(const FVector2D& P) const
{
    for (const FLimaParkData& Park : CityData.Parks)
        if (PointInsidePolygon2D(P,Park.Polygon)) return true;
    return false;
}

bool ALimaCityGenerator::SnapLocationToGround(FVector& P) const
{
    UWorld* World=GetWorld(); if(!World) return false;
    FCollisionQueryParams Q(SCENE_QUERY_STAT(KalexGroundTrace),true,this); Q.bTraceComplex=true;
    const FVector Start(P.X,P.Y,P.Z+GroundTraceHeight), End(P.X,P.Y,P.Z-GroundTraceDepth);
    TArray<FHitResult> Hits;
    if(!World->LineTraceMultiByChannel(Hits,Start,End,ECC_Visibility,Q)) return false;
    for(const FHitResult& H:Hits) if(H.GetActor() && H.GetActor()->IsA<ALandscapeProxy>()) { P.Z=H.ImpactPoint.Z; return true; }
    const FHitResult* Lowest=nullptr;
    for(const FHitResult& H:Hits) if(H.bBlockingHit && (!Lowest || H.ImpactPoint.Z<Lowest->ImpactPoint.Z)) Lowest=&H;
    if(Lowest){P.Z=Lowest->ImpactPoint.Z; return true;} return false;
}

bool ALimaCityGenerator::SampleBuildingGround(const TArray<FVector2D>& LocalFootprint, float& OutMinZ, float& OutMaxZ, float& OutAverageZ) const
{
    if (LocalFootprint.Num() == 0) return false;
    OutMinZ = TNumericLimits<float>::Max();
    OutMaxZ = -TNumericLimits<float>::Max();
    double Sum = 0.0;
    int32 Valid = 0;
    for (const FVector2D& Local : LocalFootprint)
    {
        FVector WorldP = GetActorTransform().TransformPosition(FVector(Local.X, Local.Y, 0.f));
        if (!SnapLocationToGround(WorldP)) continue;
        OutMinZ = FMath::Min(OutMinZ, WorldP.Z);
        OutMaxZ = FMath::Max(OutMaxZ, WorldP.Z);
        Sum += WorldP.Z;
        ++Valid;
    }
    if (Valid == 0) return false;
    OutAverageZ = static_cast<float>(Sum / Valid);
    return true;
}

bool ALimaCityGenerator::SpawnProceduralBuilding(
    const FVector2D& LocalCenter, float WidthMeters, float DepthMeters, float YawDeg,
    int32 Floors, bool bCommercial, bool bUnfinished, int32 Seed, float Coverage,
    float RoadSetbackMeters, bool bSnapGround, EBuildingRejectReason* OutRejectReason,
    const TArray<FVector2D>* AllowedLotFootprint, const FString& GenerationId)
{
    if(OutRejectReason) *OutRejectReason=EBuildingRejectReason::None;
    UWorld* World=GetWorld(); if(!World){if(OutRejectReason)*OutRejectReason=EBuildingRejectReason::Spawn;return false;}

    const float W=FMath::Clamp(WidthMeters*UnitsPerMeter*Coverage,300.f,2000.f);
    const float D=FMath::Clamp(DepthMeters*UnitsPerMeter*Coverage,400.f,3000.f);
    const TArray<FVector2D> Footprint=MakeBuildingFootprint(LocalCenter,W*.5f,D*.5f,YawDeg);

    if(AllowedLotFootprint && !IsFootprintInsidePolygon(Footprint,*AllowedLotFootprint))
    { if(OutRejectReason)*OutRejectReason=EBuildingRejectReason::OutsideLot; return false; }
    if(!IsBuildingClearOfRoads(LocalCenter,W*.5f,D*.5f,YawDeg,RoadSetbackMeters))
    { if(OutRejectReason)*OutRejectReason=EBuildingRejectReason::Road; return false; }
    for(const FVector2D& P:Footprint) if(IsPointInsidePark(P))
    { if(OutRejectReason)*OutRejectReason=EBuildingRejectReason::Park; return false; }
    if(DoesFootprintOverlapGenerated(Footprint))
    { if(OutRejectReason)*OutRejectReason=EBuildingRejectReason::BuildingOverlap; return false; }

    FVector WorldLocation=GetActorTransform().TransformPosition(FVector(LocalCenter.X,LocalCenter.Y,0.f));
    float GroundMinZ = WorldLocation.Z, GroundMaxZ = WorldLocation.Z, GroundAverageZ = WorldLocation.Z;
    float GroundDelta = 0.f;
    if (bSnapGround)
    {
        if (bAdaptBuildingsToTerrain && bSampleBuildingFootprintGround)
        {
            if (!SampleBuildingGround(Footprint, GroundMinZ, GroundMaxZ, GroundAverageZ))
            { if(OutRejectReason)*OutRejectReason=EBuildingRejectReason::Ground; return false; }
            GroundDelta = GroundMaxZ - GroundMinZ;
            if (MaxBuildingGroundDeltaMeters > 0.f && GroundDelta > MaxBuildingGroundDeltaMeters * UnitsPerMeter)
            { if(OutRejectReason)*OutRejectReason=EBuildingRejectReason::Ground; return false; }
            // Highest corner guarantees that no part of the house sinks into the terrain.
            WorldLocation.Z = GroundMaxZ;
        }
        else if (!SnapLocationToGround(WorldLocation))
        { if(OutRejectReason)*OutRejectReason=EBuildingRejectReason::Ground; return false; }
    }

    const FRotator Rotation(0.f,GetActorRotation().Yaw+YawDeg,0.f);
    FActorSpawnParameters Params; Params.Owner=this; Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ALimaBuildingGenerator* B=World->SpawnActor<ALimaBuildingGenerator>(ALimaBuildingGenerator::StaticClass(),WorldLocation,Rotation,Params);
    if(!B){if(OutRejectReason)*OutRejectReason=EBuildingRejectReason::Spawn;return false;}
#if WITH_EDITOR
    B->SetActorLabel(GenerationId.IsEmpty()?FString::Printf(TEXT("BLD_%08d"),Seed):FString::Printf(TEXT("BLD_%s"),*GenerationId));
#endif
    B->Tags.Add(TEXT("KalexGeneratedCity")); B->bGenerateOnConstruction=false; B->GenerationId=GenerationId;
    B->Seed=Seed; B->BuildingWidth=W; B->BuildingDepth=D; B->Floors=FMath::Clamp(Floors,1,6);
    B->bHasStorefront=bCommercial; B->bHasGarage=!bCommercial&&((Seed%100)<48); B->bUnfinishedRoof=bUnfinished;
    B->bHasWaterTank=((Seed/7)%100)<42; B->bHasBrickStack=bUnfinished&&(((Seed/13)%100)<35); B->bVaryFinishBySeed=true;
    // V5.2.2: foundation is additive and only appears on sloped lots. Existing flat-city behavior is unchanged.
    B->bGenerateFoundation = bAdaptBuildingsToTerrain && GroundDelta >= FoundationMinSlopeMeters * UnitsPerMeter;
    B->FoundationDepth = B->bGenerateFoundation ? GroundDelta : 0.f;
    B->GenerateBuilding(); OccupiedBuildingFootprints.Add(Footprint); ++GeneratedBuildings; return true;
}

void ALimaCityGenerator::GenerateManualZones()
{
    UWorld* World=GetWorld(); if(!World) return;
    for (TActorIterator<AKalexUrbanZone> It(World); It; ++It) GenerateZone(*It);
}

void ALimaCityGenerator::GenerateZone(AKalexUrbanZone* Zone)
{
    if(!Zone || !Zone->ZoneBounds) return;
    FRandomStream R(Zone->Seed);
    const FVector Ext=Zone->ZoneBounds->GetScaledBoxExtent();
    const FTransform ZT=Zone->ZoneBounds->GetComponentTransform();
    const float MinW=FMath::Max(3.f,Zone->MinLotWidthMeters)*UnitsPerMeter;
    const float MaxW=FMath::Max(MinW,Zone->MaxLotWidthMeters*UnitsPerMeter);
    const float MinD=FMath::Max(4.f,Zone->MinLotDepthMeters)*UnitsPerMeter;
    const float MaxD=FMath::Max(MinD,Zone->MaxLotDepthMeters*UnitsPerMeter);
    float Y=-Ext.Y;
    int32 Index=0;
    while(Y < Ext.Y-MinD)
    {
        const float D=FMath::Min(R.FRandRange(MinD,MaxD),Ext.Y-Y);
        float X=-Ext.X;
        while(X < Ext.X-MinW)
        {
            const float W=FMath::Min(R.FRandRange(MinW,MaxW),Ext.X-X);
            const FVector ZoneLocal(X+W*.5f,Y+D*.5f,0.f);
            const FVector World=ZT.TransformPosition(ZoneLocal);
            const FVector CityLocal3=GetActorTransform().InverseTransformPosition(World);
            const FVector2D C(CityLocal3.X,CityLocal3.Y);
            const int32 S=Zone->Seed + Index++*7919;
            if(R.FRand() >= Zone->EmptyLotProbability && (!Zone->bAvoidParks || !IsPointInsidePark(C)))
            {
                bool Commercial = Zone->ZoneType==EKalexUrbanZoneType::Commercial ||
                    (Zone->ZoneType==EKalexUrbanZoneType::Mixed && R.FRand()<.5f) || R.FRand()<Zone->CommercialProbability;
                const bool Unfinished=R.FRand()<Zone->UnfinishedRoofProbability;
                const int32 Floors=R.RandRange(FMath::Min(Zone->MinFloors,Zone->MaxFloors),FMath::Max(Zone->MinFloors,Zone->MaxFloors));
                const float Setback=Zone->bRespectExistingRoads?Zone->RoadSetbackMeters:-1000.f;
                SpawnProceduralBuilding(C,W/UnitsPerMeter,D/UnitsPerMeter,Zone->GetActorRotation().Yaw-GetActorRotation().Yaw,
                    Floors,Commercial,Unfinished,S,Zone->BuildingCoverage,Setback,Zone->bSnapToTerrain,nullptr,nullptr,
                    FString::Printf(TEXT("ZONE_%s_%d"),*Zone->ZoneName,Index-1));
            }
            X+=W;
        }
        Y+=D;
    }
    UE_LOG(LogTemp,Warning,TEXT("KALEX ZONE GENERATED | %s | candidates=%d"),*Zone->ZoneName,Index);
}

void ALimaCityGenerator::DrawBlocks()
{
    if (!GetWorld()) return;
    for (const FLimaBlockData& B : CityData.Blocks)
        for (int32 I=0; I<B.Polygon.Num(); ++I)
        {
            const FVector2D A=B.Polygon[I], C=B.Polygon[(I+1)%B.Polygon.Num()];
            DrawDebugLine(GetWorld(), GetActorTransform().TransformPosition(FVector(A.X,A.Y,25)), GetActorTransform().TransformPosition(FVector(C.X,C.Y,25)), FColor::Yellow, true, -1, 0, DebugLineThickness);
        }
}

void ALimaCityGenerator::DrawLots()
{
    if (!GetWorld()) return;
    for (const FLimaCityLotData& L : CityData.Lots)
        for (int32 I=0; I<L.Footprint.Num(); ++I)
        {
            const FVector2D A=L.Footprint[I], C=L.Footprint[(I+1)%L.Footprint.Num()];
            DrawDebugLine(GetWorld(), GetActorTransform().TransformPosition(FVector(A.X,A.Y,35)), GetActorTransform().TransformPosition(FVector(C.X,C.Y,35)), FColor::Blue, true, -1, 0, FMath::Max(3.0f,DebugLineThickness*0.5f));
        }
}

/*
 * ================================================================
 * ROAD WIDTH
 * ================================================================
 */

float ALimaCityGenerator::GetRoadWidthMeters(
    const FLimaRoadData& Road
) const
{
    /*
     * Si OSM proporciona un width razonable,
     * lo respetamos.
     */

    if (Road.Width > 0.5f)
    {
        return Road.Width;
    }


    /*
     * Si no existe width, inferimos un ancho
     * aproximado según jerarquía OSM.
     */

    if (
        Road.Type == TEXT("motorway")
        ||
        Road.Type == TEXT("trunk")
    )
    {
        return 14.0f;
    }


    if (Road.Type == TEXT("primary"))
    {
        return 11.0f;
    }


    if (Road.Type == TEXT("secondary"))
    {
        return 9.0f;
    }


    if (Road.Type == TEXT("tertiary"))
    {
        return 8.0f;
    }


    if (Road.Type == TEXT("residential"))
    {
        return 6.5f;
    }


    if (
        Road.Type == TEXT("service")
        ||
        Road.Type == TEXT("living_street")
    )
    {
        return 5.0f;
    }


    /*
     * Segunda opción:
     * inferir por número de carriles.
     */

    if (Road.Lanes > 0)
    {
        return FMath::Max(
            4.0f,
            static_cast<float>(Road.Lanes)
                * 3.2f
        );
    }


    return DefaultRoadWidthMeters;
}


/*
 * ================================================================
 * GENERATE PHYSICAL ROADS
 * ================================================================
 */

void ALimaCityGenerator::GenerateRoadMeshes()
{
    GeneratedRoadSplines = 0;
    GeneratedRoadSegments = 0;


    if (!RoadMesh)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "KALEX CITY: RoadMesh is NULL. "
                "Assign SM_SJL_Road_Asphalt_2Lane "
                "to LimaCityGenerator."
            )
        );

        return;
    }


    if (RoadMeshBaseWidthMeters <= KINDA_SMALL_NUMBER)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "KALEX CITY: "
                "RoadMeshBaseWidthMeters must be > 0"
            )
        );

        return;
    }


    /*
     * ============================================================
     * ONE SPLINE PER OSM ROAD
     * ============================================================
     */

    for (
        int32 RoadIndex = 0;
        RoadIndex < CityData.Roads.Num();
        ++RoadIndex
    )
    {
        const FLimaRoadData& Road =
            CityData.Roads[RoadIndex];


        if (Road.Points.Num() < 2)
        {
            continue;
        }


        /*
         * --------------------------------------------------------
         * SPLINE
         * --------------------------------------------------------
         */

        USplineComponent* Spline =
            NewObject<USplineComponent>(
                this,
                *FString::Printf(
                    TEXT("GeneratedRoadSpline_%d"),
                    RoadIndex
                ),
                RF_Transactional
            );


        if (!Spline)
        {
            continue;
        }


        Spline->SetupAttachment(SceneRoot);

        AddInstanceComponent(Spline);

        Spline->RegisterComponent();

        Spline->SetMobility(
            EComponentMobility::Static
        );

        Spline->ClearSplinePoints(false);


        /*
         * --------------------------------------------------------
         * ADD OSM POINTS
         * --------------------------------------------------------
         */

        for (
            int32 PointIndex = 0;
            PointIndex < Road.Points.Num();
            ++PointIndex
        )
        {
            FVector Point = Road.Points[PointIndex];

            // V5.2.2 is opt-in for roads to preserve the already-correct V5.2.1 layout by default.
            if (bSnapRoadsToTerrain)
            {
                FVector WorldPoint = GetActorTransform().TransformPosition(Point);
                if (SnapLocationToGround(WorldPoint))
                {
                    Point = GetActorTransform().InverseTransformPosition(WorldPoint);
                    Point.Z += RoadTerrainOffsetMeters * UnitsPerMeter;
                }
                else
                {
                    Point.Z += RoadZOffset;
                }
            }
            else
            {
                Point.Z += RoadZOffset;
            }


            Spline->AddSplinePoint(
                Point,
                ESplineCoordinateSpace::Local,
                false
            );
        }


        /*
         * Curve permite que calles con varios nodos
         * tengan transiciones más suaves.
         *
         * Si posteriormente encontramos deformaciones
         * en intersecciones podemos cambiarlo por Linear.
         */

        for (
            int32 PointIndex = 0;
            PointIndex <
                Spline->GetNumberOfSplinePoints();
            ++PointIndex
        )
        {
            Spline->SetSplinePointType(
                PointIndex,
                ESplinePointType::Curve,
                false
            );
        }


        Spline->UpdateSpline();


        /*
         * --------------------------------------------------------
         * ROAD WIDTH
         * --------------------------------------------------------
         */

        const float RoadWidthMeters =
            GetRoadWidthMeters(Road);


        /*
         * IMPORTANTE:
         *
         * SplineMesh Scale trabaja sobre las dimensiones
         * originales del StaticMesh.
         *
         * Aquí asumimos:
         *
         * X = longitud
         * Y = ancho
         * Z = grosor
         */

        const float WidthScale =
            RoadWidthMeters /
            RoadMeshBaseWidthMeters;


        /*
         * --------------------------------------------------------
         * CREATE SEGMENTS
         * --------------------------------------------------------
         */

        const int32 SegmentCount =
            Spline->GetNumberOfSplinePoints() - 1;


        for (
            int32 SegmentIndex = 0;
            SegmentIndex < SegmentCount;
            ++SegmentIndex
        )
        {
            FVector StartPosition;
            FVector StartTangent;

            FVector EndPosition;
            FVector EndTangent;


            Spline->GetLocationAndTangentAtSplinePoint(
                SegmentIndex,
                StartPosition,
                StartTangent,
                ESplineCoordinateSpace::Local
            );


            Spline->GetLocationAndTangentAtSplinePoint(
                SegmentIndex + 1,
                EndPosition,
                EndTangent,
                ESplineCoordinateSpace::Local
            );


            /*
             * Evitar segmentos degenerados.
             */

            if (
                FVector::DistSquared(
                    StartPosition,
                    EndPosition
                )
                < 1.0f
            )
            {
                continue;
            }


            USplineMeshComponent* Segment =
                NewObject<USplineMeshComponent>(
                    this,
                    *FString::Printf(
                        TEXT(
                            "GeneratedRoadMesh_%d_%d"
                        ),
                        RoadIndex,
                        SegmentIndex
                    ),
                    RF_Transactional
                );


            if (!Segment)
            {
                continue;
            }


            Segment->SetupAttachment(Spline);

            Segment->SetStaticMesh(
                RoadMesh
            );


            /*
             * Nuestro road mesh debe avanzar sobre X.
             */

            Segment->SetForwardAxis(
                ESplineMeshAxis::X,
                false
            );


            /*
             * Geometría spline.
             */

            Segment->SetStartAndEnd(
                StartPosition,
                StartTangent,
                EndPosition,
                EndTangent,
                false
            );


            /*
             * Para ForwardAxis X:
             *
             * FVector2D.X controla Y
             * FVector2D.Y controla Z
             *
             * Es decir:
             *
             * X = ancho
             * Y = grosor/altura
             */

            Segment->SetStartScale(
                FVector2D(
                    WidthScale,
                    1.0f
                ),
                false
            );


            Segment->SetEndScale(
                FVector2D(
                    WidthScale,
                    1.0f
                ),
                false
            );


            /*
             * Collision.
             */

            Segment->SetCollisionEnabled(
                bRoadCollision
                ? ECollisionEnabled::QueryAndPhysics
                : ECollisionEnabled::NoCollision
            );


            /*
             * Static porque la ciudad no se moverá
             * durante gameplay.
             */

            Segment->SetMobility(
                EComponentMobility::Static
            );


            AddInstanceComponent(
                Segment
            );

            Segment->RegisterComponent();


            /*
             * Actualizamos solo después de configurar
             * todas las propiedades.
             */

            Segment->UpdateMesh();


            ++GeneratedRoadSegments;
        }


        ++GeneratedRoadSplines;
    }


    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "KALEX CITY ROADS GENERATED | "
            "Splines=%d Segments=%d"
        ),
        GeneratedRoadSplines,
        GeneratedRoadSegments
    );
}


/*
 * ================================================================
 * ROAD DEBUG COLOR
 * ================================================================
 */

FColor ALimaCityGenerator::GetRoadDebugColor(
    const FLimaRoadData& Road
) const
{
    if (
        Road.Type == TEXT("motorway")
        ||
        Road.Type == TEXT("trunk")
        ||
        Road.Type == TEXT("primary")
        ||
        Road.Type == TEXT("secondary")
    )
    {
        return FColor::Red;
    }


    if (Road.Type == TEXT("tertiary"))
    {
        return FColor::Orange;
    }


    if (Road.Type == TEXT("residential"))
    {
        return FColor::Yellow;
    }


    if (Road.Type == TEXT("service"))
    {
        return FColor::Silver;
    }


    return FColor::White;
}


/*
 * ================================================================
 * ROAD DEBUG
 * ================================================================
 */

void ALimaCityGenerator::DrawRoadDebug()
{
    UWorld* World =
        GetWorld();

    if (!World)
    {
        return;
    }


    for (
        const FLimaRoadData& Road :
        CityData.Roads
    )
    {
        const FColor Color =
            GetRoadDebugColor(Road);


        for (
            int32 i = 0;
            i < Road.Points.Num() - 1;
            ++i
        )
        {
            FVector A =
                GetActorTransform()
                .TransformPosition(
                    Road.Points[i]
                );


            FVector B =
                GetActorTransform()
                .TransformPosition(
                    Road.Points[i + 1]
                );


            A.Z += RoadZOffset + 10.0f;
            B.Z += RoadZOffset + 10.0f;


            DrawDebugLine(
                World,
                A,
                B,
                Color,
                true,
                -1.0f,
                0,
                FMath::Max(
                    DebugLineThickness,
                    GetRoadWidthMeters(Road)
                        * UnitsPerMeter
                        * 0.02f
                )
            );
        }
    }
}


/*
 * ================================================================
 * BUILDING FOOTPRINT DEBUG
 * ================================================================
 */

void ALimaCityGenerator::DrawBuildingFootprints()
{
    UWorld* World =
        GetWorld();

    if (!World)
    {
        return;
    }


    for (
        const FLimaBuildingData& Building :
        CityData.Buildings
    )
    {
        const int32 Count =
            Building.Footprint.Num();


        if (Count < 3)
        {
            continue;
        }


        for (
            int32 i = 0;
            i < Count;
            ++i
        )
        {
            const FVector2D A2 =
                Building.Footprint[i];


            const FVector2D B2 =
                Building.Footprint[
                    (i + 1) % Count
                ];


            FVector A(
                A2.X,
                A2.Y,
                20.0f
            );


            FVector B(
                B2.X,
                B2.Y,
                20.0f
            );


            A =
                GetActorTransform()
                .TransformPosition(A);


            B =
                GetActorTransform()
                .TransformPosition(B);


            DrawDebugLine(
                World,
                A,
                B,
                FColor::Cyan,
                true,
                -1.0f,
                0,
                20.0f
            );
        }
    }
}


/*
 * ================================================================
 * PARK DEBUG
 * ================================================================
 */

void ALimaCityGenerator::DrawParks()
{
    UWorld* World =
        GetWorld();

    if (!World)
    {
        return;
    }


    for (
        const FLimaParkData& Park :
        CityData.Parks
    )
    {
        const int32 Count =
            Park.Polygon.Num();


        if (Count < 3)
        {
            continue;
        }


        for (
            int32 i = 0;
            i < Count;
            ++i
        )
        {
            const FVector2D A2 =
                Park.Polygon[i];


            const FVector2D B2 =
                Park.Polygon[
                    (i + 1) % Count
                ];


            FVector A(
                A2.X,
                A2.Y,
                30.0f
            );


            FVector B(
                B2.X,
                B2.Y,
                30.0f
            );


            A =
                GetActorTransform()
                .TransformPosition(A);


            B =
                GetActorTransform()
                .TransformPosition(B);


            DrawDebugLine(
                World,
                A,
                B,
                FColor::Green,
                true,
                -1.0f,
                0,
                30.0f
            );
        }
    }
}


/*
 * ================================================================
 * CLEAR GENERATED CITY
 * ================================================================
 */

void ALimaCityGenerator::ClearGeneratedCity()
{
    if (UWorld* World = GetWorld())
    {
        TArray<AActor*> ToDestroy;
        for (TActorIterator<AActor> It(World); It; ++It)
        {
            AActor* Actor=*It;
            if (!Actor || !Actor->ActorHasTag(TEXT("KalexGeneratedCity"))) continue;
            if (ALimaBuildingGenerator* B=Cast<ALimaBuildingGenerator>(Actor))
            {
                if (B->bManualOverride || B->bLockFromRegeneration) continue;
            }
            ToDestroy.Add(Actor);
        }
        for (AActor* A : ToDestroy) A->Destroy();
    }
    GeneratedBuildings = 0;
    /*
     * ============================================================
     * REMOVE DEBUG LINES
     * ============================================================
     */

    if (UWorld* World = GetWorld())
    {
        FlushPersistentDebugLines(
            World
        );
    }


    /*
     * ============================================================
     * REMOVE GENERATED COMPONENTS
     * ============================================================
     */

    TArray<UActorComponent*> Components;

    GetComponents(
        Components
    );


    /*
     * Copiamos primero el array porque DestroyComponent()
     * modifica la colección interna del actor.
     */

    TArray<UActorComponent*> ComponentsToDestroy;


    for (
        UActorComponent* Component :
        Components
    )
    {
        if (!Component)
        {
            continue;
        }


        /*
         * Nunca destruir SceneRoot.
         */

        if (Component == SceneRoot)
        {
            continue;
        }


        /*
         * Solo eliminamos componentes creados
         * por nuestro generador.
         */

        const FString ComponentName =
            Component->GetName();


        if (
            ComponentName.StartsWith(
                TEXT("GeneratedRoadSpline_")
            )
            ||
            ComponentName.StartsWith(
                TEXT("GeneratedRoadMesh_")
            )
        )
        {
            ComponentsToDestroy.Add(
                Component
            );
        }
    }


    for (
        UActorComponent* Component :
        ComponentsToDestroy
    )
    {
        if (Component)
        {
            Component->DestroyComponent();
        }
    }


    /*
     * ============================================================
     * RESET STATS
     * ============================================================
     */

    LoadedRoads = 0;
    LoadedBuildings = 0;
    LoadedParks = 0;

    GeneratedRoadSplines = 0;
    GeneratedRoadSegments = 0;


    /*
     * Reset data.
     */

    CityData = FLimaCityData();


    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "KALEX CITY: Generated city cleared"
        )
    );
}
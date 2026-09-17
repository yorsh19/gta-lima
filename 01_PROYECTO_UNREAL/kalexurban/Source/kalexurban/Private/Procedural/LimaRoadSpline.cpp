#include "Procedural/LimaRoadSpline.h"

#include "Procedural/LimaBuildingGenerator.h"
#include "Procedural/LimaUtilityPole.h"
#include "Procedural/LimaCable.h"
#include "Procedural/LimaRoadSegment.h"

#include "Data/LimaDistrictStyle.h"

ALimaRoadSpline::ALimaRoadSpline()
{
    PrimaryActorTick.bCanEverTick = false;

    Root =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("Root")
        );

    RootComponent = Root;

    RoadSpline =
        CreateDefaultSubobject<USplineComponent>(
            TEXT("RoadSpline")
        );

    RoadSpline->SetupAttachment(Root);
}

void ALimaRoadSpline::ClearGenerated()
{
    for (AActor* Actor : GeneratedActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }

    GeneratedActors.Empty();
    GeneratedPoles.Empty();
}

void ALimaRoadSpline::GenerateStreet()
{
    if (!DistrictStyle)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("%s: DistrictStyle missing."),
            *GetName()
        );

        return;
    }

    ClearGenerated();

    FRandomStream Random(Seed);

    if (bGenerateRoad)
    {
        GenerateRoad();
    }

    if (bGenerateBuildings)
    {
        if (bGenerateLeft)
        {
            GenerateSide(Random, -1.f);
        }

        if (bGenerateRight)
        {
            GenerateSide(Random, 1.f);
        }
    }

    if (bGenerateUtilities)
    {
        GenerateUtilityPoles(Random);
    }

    if (bGenerateCables)
    {
        GenerateCables();
    }
}

void ALimaRoadSpline::GenerateRoad()
{
    if (!RoadSegmentClass)
    {
        return;
    }

    const int32 NumPoints =
        RoadSpline->GetNumberOfSplinePoints();

    for (int32 Index = 0;
         Index < NumPoints - 1;
         ++Index)
    {
        const FVector Start =
            RoadSpline->GetLocationAtSplinePoint(
                Index,
                ESplineCoordinateSpace::World
            );

        const FVector End =
            RoadSpline->GetLocationAtSplinePoint(
                Index + 1,
                ESplineCoordinateSpace::World
            );

        const FVector StartTangent =
            RoadSpline->GetTangentAtSplinePoint(
                Index,
                ESplineCoordinateSpace::World
            );

        const FVector EndTangent =
            RoadSpline->GetTangentAtSplinePoint(
                Index + 1,
                ESplineCoordinateSpace::World
            );

        ALimaRoadSegment* Segment =
            GetWorld()->SpawnActor<ALimaRoadSegment>(
                RoadSegmentClass,
                FVector::ZeroVector,
                FRotator::ZeroRotator
            );

        if (!Segment)
        {
            continue;
        }

        Segment->RoadWidth =
            RoadWidth;

        Segment->StartPosition =
            Start;

        Segment->EndPosition =
            End;

        Segment->StartTangent =
            StartTangent;

        Segment->EndTangent =
            EndTangent;

        Segment->GenerateRoadSegment();

        Segment->Tags.Add(
            TEXT("LimaGenerated")
        );

        GeneratedActors.Add(Segment);
    }
}

void ALimaRoadSpline::GenerateSide(
    FRandomStream& Random,
    float SideMultiplier)
{
    if (!BuildingClass)
    {
        return;
    }

    const float SplineLength =
        RoadSpline->GetSplineLength();

    float Distance = 0.f;

    while (Distance < SplineLength)
    {
        const float Width =
            Random.FRandRange(
                DistrictStyle->MinLotWidth,
                DistrictStyle->MaxLotWidth
            );

        const float Depth =
            Random.FRandRange(
                DistrictStyle->MinLotDepth,
                DistrictStyle->MaxLotDepth
            );

        const float CenterDistance =
            FMath::Min(
                Distance + Width * 0.5f,
                SplineLength
            );

        FVector Position =
            RoadSpline
            ->GetLocationAtDistanceAlongSpline(
                CenterDistance,
                ESplineCoordinateSpace::World
            );

        const FVector Right =
            RoadSpline
            ->GetRightVectorAtDistanceAlongSpline(
                CenterDistance,
                ESplineCoordinateSpace::World
            );

        const FVector Direction =
            RoadSpline
            ->GetDirectionAtDistanceAlongSpline(
                CenterDistance,
                ESplineCoordinateSpace::World
            );

        const float Offset =
            RoadWidth * 0.5f +
            SidewalkWidth +
            BuildingOffset +
            Depth * 0.5f;

        Position +=
            Right *
            Offset *
            SideMultiplier;

        FRotator Rotation =
            Direction.Rotation();

        if (SideMultiplier < 0.f)
        {
            Rotation.Yaw += 180.f;
        }

        if (Random.FRand() <=
            DistrictStyle->BuildingDensity)
        {
            ALimaBuildingGenerator* Building =
                GetWorld()
                ->SpawnActor<ALimaBuildingGenerator>(
                    BuildingClass,
                    Position,
                    Rotation
                );

            if (Building)
            {
                Building->BuildingWidth = Width;
                Building->BuildingDepth = Depth;

                Building->Floors =
                    Random.RandRange(
                        DistrictStyle->MinFloors,
                        DistrictStyle->MaxFloors
                    );

                Building->Seed =
                    Random.RandRange(
                        1,
                        INT32_MAX
                    );

                Building->bHasStorefront =
                    Random.FRand() <
                    DistrictStyle->CommercialChance;

                Building->bUnfinishedRoof =
                    Random.FRand() <
                    DistrictStyle->UnfinishedChance;

                Building->GenerateBuilding();

                Building->Tags.Add(
                    TEXT("LimaGenerated")
                );

                GeneratedActors.Add(Building);
            }
        }

        Distance += Width;
    }
}

void ALimaRoadSpline::GenerateUtilityPoles(
    FRandomStream& Random)
{
    if (!UtilityPoleClass)
    {
        return;
    }

    const float Length =
        RoadSpline->GetSplineLength();

    float Distance = 0.f;

    while (Distance < Length)
    {
        FVector Position =
            RoadSpline
            ->GetLocationAtDistanceAlongSpline(
                Distance,
                ESplineCoordinateSpace::World
            );

        const FVector Right =
            RoadSpline
            ->GetRightVectorAtDistanceAlongSpline(
                Distance,
                ESplineCoordinateSpace::World
            );

        Position +=
            Right *
            (
                RoadWidth * 0.5f +
                SidewalkWidth * 0.5f
            );

        FRotator Rotation =
            RoadSpline
            ->GetDirectionAtDistanceAlongSpline(
                Distance,
                ESplineCoordinateSpace::World
            )
            .Rotation();

        ALimaUtilityPole* Pole =
            GetWorld()
            ->SpawnActor<ALimaUtilityPole>(
                UtilityPoleClass,
                Position,
                Rotation
            );

        if (Pole)
        {
            Pole->Seed =
                Random.RandRange(
                    1,
                    INT32_MAX
                );

            Pole->GeneratePole();

            Pole->Tags.Add(
                TEXT("LimaGenerated")
            );

            GeneratedActors.Add(Pole);
            GeneratedPoles.Add(Pole);
        }

        Distance +=
            Random.FRandRange(
                DistrictStyle->PoleMinSpacing,
                DistrictStyle->PoleMaxSpacing
            );
    }
}

void ALimaRoadSpline::GenerateCables()
{
    if (!CableClass ||
        GeneratedPoles.Num() < 2)
    {
        return;
    }

    for (int32 Index = 0;
         Index < GeneratedPoles.Num() - 1;
         ++Index)
    {
        ALimaUtilityPole* PoleA =
            GeneratedPoles[Index];

        ALimaUtilityPole* PoleB =
            GeneratedPoles[Index + 1];

        if (!PoleA || !PoleB)
        {
            continue;
        }

        // 3 líneas eléctricas
        for (int32 PowerIndex = 0;
             PowerIndex < 3;
             ++PowerIndex)
        {
            ALimaCable* Cable =
                GetWorld()
                ->SpawnActor<ALimaCable>(
                    CableClass,
                    FVector::ZeroVector,
                    FRotator::ZeroRotator
                );

            if (!Cable)
            {
                continue;
            }

            Cable->StartPosition =
                PoleA->GetPowerLocation(
                    PowerIndex
                );

            Cable->EndPosition =
                PoleB->GetPowerLocation(
                    PowerIndex
                );

            Cable->Sag =
                50.f + PowerIndex * 10.f;

            Cable->GenerateCable();

            Cable->Tags.Add(
                TEXT("LimaGenerated")
            );

            GeneratedActors.Add(Cable);
        }

        // Telecom
        ALimaCable* Telecom =
            GetWorld()
            ->SpawnActor<ALimaCable>(
                CableClass,
                FVector::ZeroVector,
                FRotator::ZeroRotator
            );

        if (Telecom)
        {
            Telecom->StartPosition =
                PoleA->GetTelecomLocation();

            Telecom->EndPosition =
                PoleB->GetTelecomLocation();

            Telecom->Sag = 110.f;

            Telecom->GenerateCable();

            Telecom->Tags.Add(
                TEXT("LimaGenerated")
            );

            GeneratedActors.Add(Telecom);
        }
    }
}
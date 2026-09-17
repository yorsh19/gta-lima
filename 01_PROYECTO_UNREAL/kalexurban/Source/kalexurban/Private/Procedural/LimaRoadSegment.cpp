#include "Procedural/LimaRoadSegment.h"

#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

ALimaRoadSegment::ALimaRoadSegment()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
}

void ALimaRoadSegment::GenerateRoadSegment()
{
    if (!RoadMesh)
    {
        return;
    }

    if (GeneratedRoad)
    {
        GeneratedRoad->DestroyComponent();
        GeneratedRoad = nullptr;
    }

    GeneratedRoad =
        NewObject<USplineMeshComponent>(this);

    GeneratedRoad->SetupAttachment(Root);
    GeneratedRoad->SetStaticMesh(RoadMesh);

    GeneratedRoad->SetMobility(
        EComponentMobility::Static
    );

    GeneratedRoad->SetForwardAxis(
        ESplineMeshAxis::X
    );

    if (RoadMaterial)
    {
        GeneratedRoad->SetMaterial(
            0,
            RoadMaterial
        );
    }

    GeneratedRoad->RegisterComponent();

    AddInstanceComponent(GeneratedRoad);

    const FVector LocalStart =
        GetActorTransform()
            .InverseTransformPosition(StartPosition);

    const FVector LocalEnd =
        GetActorTransform()
            .InverseTransformPosition(EndPosition);

    const FVector LocalStartTangent =
        GetActorTransform()
            .InverseTransformVector(StartTangent);

    const FVector LocalEndTangent =
        GetActorTransform()
            .InverseTransformVector(EndTangent);

    GeneratedRoad->SetStartAndEnd(
        LocalStart,
        LocalStartTangent,
        LocalEnd,
        LocalEndTangent,
        true
    );

    const FBoxSphereBounds Bounds =
        RoadMesh->GetBounds();

    const float MeshWidth =
        Bounds.BoxExtent.Y * 2.f;

    if (MeshWidth > KINDA_SMALL_NUMBER)
    {
        const float Scale =
            RoadWidth / MeshWidth;

        GeneratedRoad->SetStartScale(
            FVector2D(Scale, 1.f)
        );

        GeneratedRoad->SetEndScale(
            FVector2D(Scale, 1.f)
        );
    }

    GeneratedRoad->SetCollisionEnabled(
        ECollisionEnabled::QueryAndPhysics
    );
}
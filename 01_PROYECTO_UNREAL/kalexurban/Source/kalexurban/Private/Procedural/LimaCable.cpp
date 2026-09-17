#include "Procedural/LimaCable.h"

#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

ALimaCable::ALimaCable()
{
    PrimaryActorTick.bCanEverTick = false;

    Root =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("Root")
        );

    RootComponent = Root;

    CableSpline =
        CreateDefaultSubobject<USplineComponent>(
            TEXT("CableSpline")
        );

    CableSpline->SetupAttachment(Root);
}

void ALimaCable::ClearSegments()
{
    for (USplineMeshComponent* Segment :
         CableSegments)
    {
        if (IsValid(Segment))
        {
            Segment->DestroyComponent();
        }
    }

    CableSegments.Empty();
}

void ALimaCable::GenerateCable()
{
    ClearSegments();

    CableSpline->ClearSplinePoints(false);

    const FVector LocalStart =
        GetActorTransform()
            .InverseTransformPosition(
                StartPosition
            );

    const FVector LocalEnd =
        GetActorTransform()
            .InverseTransformPosition(
                EndPosition
            );

    FVector Middle =
        (LocalStart + LocalEnd) * 0.5f;

    Middle.Z -= Sag;

    CableSpline->AddSplinePoint(
        LocalStart,
        ESplineCoordinateSpace::Local,
        false
    );

    CableSpline->AddSplinePoint(
        Middle,
        ESplineCoordinateSpace::Local,
        false
    );

    CableSpline->AddSplinePoint(
        LocalEnd,
        ESplineCoordinateSpace::Local,
        false
    );

    for (int32 Index = 0;
         Index < 3;
         ++Index)
    {
        CableSpline->SetSplinePointType(
            Index,
            ESplinePointType::Curve,
            false
        );
    }

    CableSpline->UpdateSpline();

    if (!CableMesh)
    {
        return;
    }

    const int32 Segments =
        CableSpline->GetNumberOfSplinePoints() - 1;

    for (int32 Index = 0;
         Index < Segments;
         ++Index)
    {
        FVector Start;
        FVector StartTangent;
        FVector End;
        FVector EndTangent;

        CableSpline
            ->GetLocationAndTangentAtSplinePoint(
                Index,
                Start,
                StartTangent,
                ESplineCoordinateSpace::Local
            );

        CableSpline
            ->GetLocationAndTangentAtSplinePoint(
                Index + 1,
                End,
                EndTangent,
                ESplineCoordinateSpace::Local
            );

        USplineMeshComponent* Segment =
            NewObject<USplineMeshComponent>(this);

        Segment->SetupAttachment(Root);
        Segment->SetStaticMesh(CableMesh);
        Segment->SetForwardAxis(
            ESplineMeshAxis::X
        );

        if (CableMaterial)
        {
            Segment->SetMaterial(
                0,
                CableMaterial
            );
        }

        Segment->SetStartAndEnd(
            Start,
            StartTangent,
            End,
            EndTangent,
            true
        );

        Segment->SetStartScale(
            FVector2D(
                Thickness,
                Thickness
            )
        );

        Segment->SetEndScale(
            FVector2D(
                Thickness,
                Thickness
            )
        );

        Segment->SetCollisionEnabled(
            ECollisionEnabled::NoCollision
        );

        Segment->RegisterComponent();

        AddInstanceComponent(Segment);

        CableSegments.Add(Segment);
    }
}
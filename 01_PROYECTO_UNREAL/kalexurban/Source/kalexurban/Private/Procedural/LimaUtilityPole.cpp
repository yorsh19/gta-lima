#include "Procedural/LimaUtilityPole.h"

ALimaUtilityPole::ALimaUtilityPole()
{
    PrimaryActorTick.bCanEverTick = false;

    Root =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("Root")
        );

    RootComponent = Root;

    PoleMesh =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("PoleMesh")
        );

    PoleMesh->SetupAttachment(Root);

    PowerA =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("PowerA")
        );

    PowerA->SetupAttachment(Root);
    PowerA->SetRelativeLocation(
        FVector(0, -40, 800)
    );

    PowerB =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("PowerB")
        );

    PowerB->SetupAttachment(Root);
    PowerB->SetRelativeLocation(
        FVector(0, 0, 800)
    );

    PowerC =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("PowerC")
        );

    PowerC->SetupAttachment(Root);
    PowerC->SetRelativeLocation(
        FVector(0, 40, 800)
    );

    TelecomA =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("TelecomA")
        );

    TelecomA->SetupAttachment(Root);
    TelecomA->SetRelativeLocation(
        FVector(0, 0, 650)
    );
}

void ALimaUtilityPole::GeneratePole()
{
    if (PoleMeshes.IsEmpty())
    {
        return;
    }

    FRandomStream Random(Seed);

    for (int32 Attempt = 0;
         Attempt < 20;
         ++Attempt)
    {
        const int32 Index =
            Random.RandRange(
                0,
                PoleMeshes.Num() - 1
            );

        if (PoleMeshes[Index])
        {
            PoleMesh->SetStaticMesh(
                PoleMeshes[Index]
            );

            return;
        }
    }
}

FVector ALimaUtilityPole::GetPowerLocation(
    int32 Index) const
{
    switch (Index)
    {
        case 0:
            return PowerA->GetComponentLocation();

        case 1:
            return PowerB->GetComponentLocation();

        default:
            return PowerC->GetComponentLocation();
    }
}

FVector ALimaUtilityPole::GetTelecomLocation() const
{
    return TelecomA->GetComponentLocation();
}
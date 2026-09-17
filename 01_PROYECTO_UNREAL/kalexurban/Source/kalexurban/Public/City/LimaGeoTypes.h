#pragma once

#include "CoreMinimal.h"
#include "LimaGeoTypes.generated.h"

USTRUCT(BlueprintType)
struct FLimaGeoPoint
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double X = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Y = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Z = 0.0;

    FVector ToUnreal(float Scale = 100.0f) const
    {
        // JSON está en metros.
        // Unreal trabaja en centímetros.
        return FVector(
            X * Scale,
            Y * Scale,
            Z * Scale
        );
    }
};
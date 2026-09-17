#pragma once

#include "CoreMinimal.h"
#include "LimaTypes.generated.h"

UENUM(BlueprintType)
enum class ELimaRoadType : uint8
{
    MainAvenue      UMETA(DisplayName="Main Avenue"),
    Secondary       UMETA(DisplayName="Secondary"),
    Residential     UMETA(DisplayName="Residential"),
    Dirt            UMETA(DisplayName="Dirt Road"),
    Alley           UMETA(DisplayName="Alley")
};

UENUM(BlueprintType)
enum class ELimaBuildingType : uint8
{
    Residential    UMETA(DisplayName="Residential"),
    Commercial     UMETA(DisplayName="Commercial"),
    Mixed           UMETA(DisplayName="Mixed"),
    Warehouse       UMETA(DisplayName="Warehouse"),
    Hero            UMETA(DisplayName="Hero")
};

UENUM(BlueprintType)
enum class ELimaBuildingFinish : uint8
{
    ExposedBrick    UMETA(DisplayName="Exposed Brick"),
    Plaster         UMETA(DisplayName="Plaster"),
    Painted         UMETA(DisplayName="Painted"),
    Unfinished      UMETA(DisplayName="Unfinished"),
    Concrete        UMETA(DisplayName="Concrete")
};

USTRUCT(BlueprintType)
struct FLimaLotData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator Rotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Width = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Depth = 1600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Floors = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Seed = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCommercial = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCornerLot = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ELimaBuildingType BuildingType =
        ELimaBuildingType::Residential;
};
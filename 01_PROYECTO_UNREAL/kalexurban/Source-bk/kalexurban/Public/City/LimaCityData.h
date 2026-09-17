#pragma once

#include "CoreMinimal.h"
#include "LimaCityData.generated.h"

USTRUCT(BlueprintType)
struct FLimaRoadData
{
    GENERATED_BODY()

    int64 Id = 0;

    FString Name;
    FString Type;
    FString Surface;

    int32 Lanes = 2;

    float Width = 6.0f;

    bool bOneWay = false;

    TArray<FVector> Points;
};



USTRUCT(BlueprintType)
struct FLimaBlockData
{
    GENERATED_BODY()
    int64 Id = 0;
    float Area = 0.0f;
    TArray<FVector2D> Polygon;
};

USTRUCT(BlueprintType)
struct FLimaCityLotData
{
    GENERATED_BODY()

    int64 Id = 0;
    int64 BlockId = 0;

    FVector2D Center = FVector2D::ZeroVector;

    float Width = 8.0f;
    float Depth = 16.0f;
    float Yaw = 0.0f;

    int32 Floors = 2;

    bool bCommercial = false;
    bool bUnfinished = false;

    TArray<FVector2D> Footprint;
};

USTRUCT(BlueprintType)
struct FLimaBuildingData
{
    GENERATED_BODY()

    int64 Id = 0;

    FString Name;
    FString Type;

    int32 Floors = 2;

    float Height = 6.0f;

    FVector2D Center = FVector2D::ZeroVector;

    TArray<FVector2D> Footprint;
};


USTRUCT(BlueprintType)
struct FLimaParkData
{
    GENERATED_BODY()

    int64 Id = 0;

    FString Name;
    FString Type;

    TArray<FVector2D> Polygon;
};


USTRUCT(BlueprintType)
struct FLimaTreeData
{
    GENERATED_BODY()

    int64 Id = 0;

    FVector Position = FVector::ZeroVector;
};


USTRUCT(BlueprintType)
struct FLimaAmenityData
{
    GENERATED_BODY()

    int64 Id = 0;

    FString Name;
    FString Type;

    FVector Position = FVector::ZeroVector;
};


USTRUCT(BlueprintType)
struct FLimaCityData
{
    GENERATED_BODY()

    FString Name;

    double OriginLatitude = 0.0;
    double OriginLongitude = 0.0;

    TArray<FLimaRoadData> Roads;
    TArray<FLimaBlockData> Blocks;
    TArray<FLimaCityLotData> Lots;
    TArray<FLimaBuildingData> Buildings;
    TArray<FLimaParkData> Parks;
    TArray<FLimaTreeData> Trees;
    TArray<FLimaAmenityData> Amenities;
};
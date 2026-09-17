#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LimaDistrictStyle.generated.h"

UCLASS(BlueprintType)
class KALEXURBAN_API ULimaDistrictStyle : public UDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
    FName DistrictName = TEXT("SJL_CantoGrande");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings")
    int32 MinFloors = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings")
    int32 MaxFloors = 4;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings")
    float MinLotWidth = 450.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings")
    float MaxLotWidth = 900.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings")
    float MinLotDepth = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings")
    float MaxLotDepth = 2200.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings",
        meta=(ClampMin="0", ClampMax="1"))
    float BuildingDensity = 0.92f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings",
        meta=(ClampMin="0", ClampMax="1"))
    float CommercialChance = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings",
        meta=(ClampMin="0", ClampMax="1"))
    float UnfinishedChance = 0.48f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings",
        meta=(ClampMin="0", ClampMax="1"))
    float ExposedBrickChance = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings",
        meta=(ClampMin="0", ClampMax="1"))
    float GarageChance = 0.30f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings",
        meta=(ClampMin="0", ClampMax="1"))
    float WaterTankChance = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Utilities")
    float PoleMinSpacing = 2500.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Utilities")
    float PoleMaxSpacing = 3500.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment")
    float VegetationDensity = 0.20f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment")
    float DebrisDensity = 0.08f;
};
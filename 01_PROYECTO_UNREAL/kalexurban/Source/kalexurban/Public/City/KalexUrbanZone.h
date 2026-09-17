#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KalexUrbanZone.generated.h"
class UBoxComponent;

UENUM(BlueprintType)
enum class EKalexUrbanZoneType : uint8 { Residential, Commercial, Mixed, Industrial };

UCLASS(Blueprintable)
class KALEXURBAN_API AKalexUrbanZone : public AActor
{
    GENERATED_BODY()
public:
    AKalexUrbanZone();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Kalex Urban|Zone") TObjectPtr<UBoxComponent> ZoneBounds;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Zone") FString ZoneName = TEXT("SJL-01");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Zone") EKalexUrbanZoneType ZoneType = EKalexUrbanZoneType::Residential;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Generation") int32 Seed = 100;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Lots", meta=(ClampMin="3.0")) float MinLotWidthMeters = 5.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Lots", meta=(ClampMin="3.0")) float MaxLotWidthMeters = 9.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Lots", meta=(ClampMin="4.0")) float MinLotDepthMeters = 10.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Lots", meta=(ClampMin="4.0")) float MaxLotDepthMeters = 18.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Buildings", meta=(ClampMin="1", ClampMax="8")) int32 MinFloors = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Buildings", meta=(ClampMin="1", ClampMax="8")) int32 MaxFloors = 4;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Buildings", meta=(ClampMin="0.4", ClampMax="1.0")) float BuildingCoverage = .90f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Buildings", meta=(ClampMin="0.0", ClampMax="1.0")) float EmptyLotProbability = .05f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Buildings", meta=(ClampMin="0.0", ClampMax="1.0")) float CommercialProbability = .15f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Buildings", meta=(ClampMin="0.0", ClampMax="1.0")) float UnfinishedRoofProbability = .30f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Restrictions") bool bRespectExistingRoads = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Restrictions") bool bAvoidParks = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Restrictions", meta=(ClampMin="0.0")) float RoadSetbackMeters = 1.5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kalex Urban|Terrain") bool bSnapToTerrain = true;
};

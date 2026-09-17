#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"

#include "Data/LimaTypes.h"

#include "LimaRoadSpline.generated.h"

class ALimaBuildingGenerator;
class ALimaUtilityPole;
class ALimaCable;
class ALimaRoadSegment;
class ULimaDistrictStyle;

UCLASS(Blueprintable)
class KALEXURBAN_API ALimaRoadSpline : public AActor
{
    GENERATED_BODY()

public:
    ALimaRoadSpline();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USplineComponent> RoadSpline;

    // ---------------- ROAD ----------------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    ELimaRoadType RoadType =
        ELimaRoadType::Residential;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    float RoadWidth = 700.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    float SidewalkWidth = 120.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    float BuildingOffset = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    TSubclassOf<ALimaRoadSegment> RoadSegmentClass;

    // ---------------- GENERATION ----------------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    int32 Seed = 15092026;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    TObjectPtr<ULimaDistrictStyle> DistrictStyle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    TSubclassOf<ALimaBuildingGenerator> BuildingClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    TSubclassOf<ALimaUtilityPole> UtilityPoleClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    TSubclassOf<ALimaCable> CableClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    bool bGenerateRoad = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    bool bGenerateBuildings = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    bool bGenerateUtilities = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    bool bGenerateCables = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    bool bGenerateLeft = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    bool bGenerateRight = true;

    UFUNCTION(CallInEditor, BlueprintCallable, Category="Lima")
    void GenerateStreet();

    UFUNCTION(CallInEditor, BlueprintCallable, Category="Lima")
    void ClearGenerated();

private:
    UPROPERTY()
    TArray<TObjectPtr<AActor>> GeneratedActors;

    UPROPERTY()
    TArray<TObjectPtr<ALimaUtilityPole>>
        GeneratedPoles;

    void GenerateRoad();
    void GenerateSide(
        FRandomStream& Random,
        float SideMultiplier
    );

    void GenerateUtilityPoles(
        FRandomStream& Random
    );

    void GenerateCables();
};
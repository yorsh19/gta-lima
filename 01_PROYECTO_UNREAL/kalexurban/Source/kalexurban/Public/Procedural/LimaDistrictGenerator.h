#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LimaDistrictGenerator.generated.h"

class ALimaRoadSpline;
class ULimaDistrictStyle;

UCLASS(Blueprintable)
class KALEXURBAN_API ALimaDistrictGenerator : public AActor
{
    GENERATED_BODY()

public:

    ALimaDistrictGenerator();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima")
    int32 Seed = 15092026;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima")
    TObjectPtr<ULimaDistrictStyle> DistrictStyle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima")
    TArray<TObjectPtr<ALimaRoadSpline>> Roads;

    UFUNCTION(CallInEditor, BlueprintCallable, Category="Lima")
    void GenerateDistrict();

    UFUNCTION(CallInEditor, BlueprintCallable, Category="Lima")
    void ClearDistrict();
};
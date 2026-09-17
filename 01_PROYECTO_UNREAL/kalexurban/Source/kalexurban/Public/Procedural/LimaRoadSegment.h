#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "LimaRoadSegment.generated.h"

UCLASS(Blueprintable)
class KALEXURBAN_API ALimaRoadSegment : public AActor
{
    GENERATED_BODY()

public:
    ALimaRoadSegment();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    TObjectPtr<UStaticMesh> RoadMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    TObjectPtr<UMaterialInterface> RoadMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    float RoadWidth = 700.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    FVector StartPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    FVector EndPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    FVector StartTangent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Road")
    FVector EndTangent;

    UFUNCTION(BlueprintCallable, Category="Lima")
    void GenerateRoadSegment();

private:
    UPROPERTY(Transient)
    TObjectPtr<USplineMeshComponent> GeneratedRoad;
};
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "LimaCable.generated.h"

UCLASS(Blueprintable)
class KALEXURBAN_API ALimaCable : public AActor
{
    GENERATED_BODY()

public:
    ALimaCable();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USplineComponent> CableSpline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Cable")
    TObjectPtr<UStaticMesh> CableMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Cable")
    TObjectPtr<UMaterialInterface> CableMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Cable")
    FVector StartPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Cable")
    FVector EndPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Cable")
    float Sag = 80.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Cable")
    float Thickness = 1.f;

    UFUNCTION(CallInEditor, BlueprintCallable, Category="Lima")
    void GenerateCable();

private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<USplineMeshComponent>>
        CableSegments;

    void ClearSegments();
};
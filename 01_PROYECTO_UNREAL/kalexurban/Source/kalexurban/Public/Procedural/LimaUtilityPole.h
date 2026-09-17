#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "LimaUtilityPole.generated.h"

UCLASS(Blueprintable)
class KALEXURBAN_API ALimaUtilityPole : public AActor
{
    GENERATED_BODY()

public:
    ALimaUtilityPole();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> PoleMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> PowerA;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> PowerB;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> PowerC;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> TelecomA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Assets")
    TArray<TObjectPtr<UStaticMesh>> PoleMeshes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lima|Generation")
    int32 Seed = 1;

    UFUNCTION(BlueprintCallable, Category="Lima")
    void GeneratePole();

    UFUNCTION(BlueprintPure, Category="Lima")
    FVector GetPowerLocation(int32 Index) const;

    UFUNCTION(BlueprintPure, Category="Lima")
    FVector GetTelecomLocation() const;
};
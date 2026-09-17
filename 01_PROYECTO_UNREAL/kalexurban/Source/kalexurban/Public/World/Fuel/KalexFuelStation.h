#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KalexFuelStation.generated.h"

class UBoxComponent;
class AKalexVehicleBase;

UCLASS()
class KALEXURBAN_API AKalexFuelStation : public AActor
{
    GENERATED_BODY()

public:
    AKalexFuelStation();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fuel Station")
    TObjectPtr<UBoxComponent> RefuelZone;

    // Lo dejamos preparado para cuando implementemos dinero.
    // 0 = repostaje gratis.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fuel Station",
              meta = (ClampMin = "0"))
    float PricePerLiter = 0.0f;

    UFUNCTION(BlueprintPure, Category = "Fuel Station")
    AKalexVehicleBase *GetVehicleInZone() const;

    UFUNCTION(BlueprintPure, Category = "Fuel Station")
    bool HasVehicle() const;

    UFUNCTION(BlueprintPure, Category = "Fuel Station")
    bool CanRefuel() const;

    UFUNCTION(BlueprintPure, Category = "Fuel Station")
    float GetLitersNeeded() const;

    UFUNCTION(BlueprintPure, Category = "Fuel Station")
    float GetRefuelCost() const;

    UFUNCTION(BlueprintCallable, Category = "Fuel Station")
    bool TryRefuel();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TObjectPtr<AKalexVehicleBase> VehicleInZone;

    UFUNCTION()
    void OnRefuelZoneBeginOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);

    UFUNCTION()
    void OnRefuelZoneEndOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex);
};
#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "KalexVehicleSeatComponent.generated.h"

class ACharacter;
UCLASS(ClassGroup=(Kalex), meta=(BlueprintSpawnableComponent))
class KALEXURBAN_API UKalexVehicleSeatComponent : public USceneComponent
{
    GENERATED_BODY()
public:
    UKalexVehicleSeatComponent();
    /** Candidate ground positions in vehicle local coordinates. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Seat")
    TArray<FVector> ExitOffsets;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Seat", meta=(ClampMin="50"))
    float EntryDistance = 350.f;
    UFUNCTION(BlueprintCallable, Category="Seat")
    bool FindSafeExit(ACharacter* Character, FVector& Location) const;
};

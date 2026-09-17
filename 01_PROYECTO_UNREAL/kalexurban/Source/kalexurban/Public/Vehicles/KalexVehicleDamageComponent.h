#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KalexVehicleDamageComponent.generated.h"

UENUM(BlueprintType)
enum class EKalexVehicleDamageState : uint8
{
    Normal,
    Damaged,
    Critical,
    Destroyed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnVehicleHealthChanged,
    float, CurrentHealth,
    float, MaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnVehicleDamageStateChanged,
    EKalexVehicleDamageState, NewState);

UCLASS(ClassGroup = (Kalex), meta = (BlueprintSpawnableComponent))
class KALEXURBAN_API UKalexVehicleDamageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UKalexVehicleDamageComponent();

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Damage")
    void ApplyVehicleDamage(float Damage);

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Damage")
    void Repair(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Damage")
    void RepairFully();

    UFUNCTION(BlueprintPure, Category = "Vehicle|Damage")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Damage")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Damage")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category = "Vehicle|Damage")
    bool IsDestroyed() const { return Health <= 0.0f; }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Damage")
    EKalexVehicleDamageState GetDamageState() const;

    UPROPERTY(BlueprintAssignable, Category = "Vehicle|Damage")
    FOnVehicleHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Vehicle|Damage")
    FOnVehicleDamageStateChanged OnDamageStateChanged;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle|Damage",
              meta = (ClampMin = "1.0"))
    float MaxHealth = 100.0f;

private:
    UPROPERTY(VisibleInstanceOnly, Category = "Vehicle|Damage")
    float Health = 100.0f;

    EKalexVehicleDamageState LastDamageState =
        EKalexVehicleDamageState::Normal;

    void RefreshDamageState();
};
#include "Vehicles/KalexVehicleDamageComponent.h"

UKalexVehicleDamageComponent::UKalexVehicleDamageComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UKalexVehicleDamageComponent::BeginPlay()
{
    Super::BeginPlay();

    Health = MaxHealth;
    LastDamageState = GetDamageState();
}

void UKalexVehicleDamageComponent::ApplyVehicleDamage(float Damage)
{
    if (Damage <= 0.0f || IsDestroyed())
        return;

    Health = FMath::Clamp(
        Health - Damage,
        0.0f,
        MaxHealth);

    OnHealthChanged.Broadcast(Health, MaxHealth);

    RefreshDamageState();
}

void UKalexVehicleDamageComponent::Repair(float Amount)
{
    if (Amount <= 0.0f)
        return;

    Health = FMath::Clamp(
        Health + Amount,
        0.0f,
        MaxHealth);

    OnHealthChanged.Broadcast(Health, MaxHealth);

    RefreshDamageState();
}

void UKalexVehicleDamageComponent::RepairFully()
{
    Health = MaxHealth;

    OnHealthChanged.Broadcast(Health, MaxHealth);

    RefreshDamageState();
}

float UKalexVehicleDamageComponent::GetHealthPercent() const
{
    return MaxHealth > 0.0f
               ? Health / MaxHealth
               : 0.0f;
}

EKalexVehicleDamageState UKalexVehicleDamageComponent::GetDamageState() const
{
    const float Percent = GetHealthPercent();

    if (Percent <= 0.0f)
        return EKalexVehicleDamageState::Destroyed;

    if (Percent <= 0.25f)
        return EKalexVehicleDamageState::Critical;

    if (Percent <= 0.60f)
        return EKalexVehicleDamageState::Damaged;

    return EKalexVehicleDamageState::Normal;
}

void UKalexVehicleDamageComponent::RefreshDamageState()
{
    const EKalexVehicleDamageState NewState = GetDamageState();

    if (NewState == LastDamageState)
        return;

    LastDamageState = NewState;

    OnDamageStateChanged.Broadcast(NewState);
}
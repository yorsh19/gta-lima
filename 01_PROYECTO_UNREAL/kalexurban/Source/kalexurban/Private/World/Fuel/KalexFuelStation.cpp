#include "World/Fuel/KalexFuelStation.h"

#include "Vehicles/KalexVehicleBase.h"

#include "Components/BoxComponent.h"

AKalexFuelStation::AKalexFuelStation()
{
    PrimaryActorTick.bCanEverTick = false;

    RefuelZone =
        CreateDefaultSubobject<UBoxComponent>(
            TEXT("RefuelZone")
        );

    RootComponent = RefuelZone;

    RefuelZone->SetBoxExtent(
        FVector(250.0f, 250.0f, 150.0f)
    );

    RefuelZone->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly
    );

    RefuelZone->SetCollisionResponseToAllChannels(
        ECR_Ignore
    );

    RefuelZone->SetCollisionResponseToChannel(
        ECC_Vehicle,
        ECR_Overlap
    );

    RefuelZone->SetGenerateOverlapEvents(true);
}

void AKalexFuelStation::BeginPlay()
{
    Super::BeginPlay();

    RefuelZone->OnComponentBeginOverlap.AddDynamic(
        this,
        &AKalexFuelStation::OnRefuelZoneBeginOverlap
    );

    RefuelZone->OnComponentEndOverlap.AddDynamic(
        this,
        &AKalexFuelStation::OnRefuelZoneEndOverlap
    );
}

void AKalexFuelStation::OnRefuelZoneBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    AKalexVehicleBase* Vehicle =
        Cast<AKalexVehicleBase>(OtherActor);

    if (!Vehicle)
    {
        return;
    }

    VehicleInZone = Vehicle;

    UE_LOG(
        LogTemp,
        Log,
        TEXT("KALEX FUEL: Vehicle entered: %s"),
        *Vehicle->GetName()
    );
}

void AKalexFuelStation::OnRefuelZoneEndOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    if (OtherActor == VehicleInZone)
    {
        UE_LOG(
            LogTemp,
            Log,
            TEXT("KALEX FUEL: Vehicle left")
        );

        VehicleInZone = nullptr;
    }
}

AKalexVehicleBase*
AKalexFuelStation::GetVehicleInZone() const
{
    return VehicleInZone;
}

bool AKalexFuelStation::HasVehicle() const
{
    return IsValid(VehicleInZone);
}

bool AKalexFuelStation::CanRefuel() const
{
    if (!IsValid(VehicleInZone))
    {
        return false;
    }

    // Obligatorio apagar el motor.
    if (VehicleInZone->IsEngineRunning())
    {
        return false;
    }

    // No repostamos si ya está lleno.
    return GetLitersNeeded() > KINDA_SMALL_NUMBER;
}

float AKalexFuelStation::GetLitersNeeded() const
{
    if (!IsValid(VehicleInZone))
    {
        return 0.0f;
    }

    return FMath::Max(
        0.0f,
        VehicleInZone->GetFuelCapacity()
        - VehicleInZone->GetFuelLiters()
    );
}

float AKalexFuelStation::GetRefuelCost() const
{
    return GetLitersNeeded() * PricePerLiter;
}

bool AKalexFuelStation::TryRefuel()
{
    if (!CanRefuel())
    {
        return false;
    }

    const float Liters = GetLitersNeeded();
    const float Cost = GetRefuelCost();

    VehicleInZone->FillFuel();

    UE_LOG(
        LogTemp,
        Log,
        TEXT(
            "KALEX FUEL: Refueled %.2f L | Cost S/ %.2f"
        ),
        Liters,
        Cost
    );

    return true;
}
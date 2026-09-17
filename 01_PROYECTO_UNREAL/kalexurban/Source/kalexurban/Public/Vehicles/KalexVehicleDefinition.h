#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "KalexVehicleDefinition.generated.h"
class USkeletalMesh;
class UAnimInstance;
class USoundBase;
class UMaterialInterface;
class UStaticMesh;

/** Immutable per-model tuning; runtime state belongs to vehicle components. */
UCLASS(BlueprintType)
class KALEXURBAN_API UKalexVehicleDefinition : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
    TObjectPtr<USkeletalMesh> Mesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
    TObjectPtr<UStaticMesh> LightsMesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
    TSubclassOf<UAnimInstance> AnimationClass;
    UPROPERTY(EditAnywhere, Category = "Physics")
    TArray<FChaosWheelSetup> Wheels;
    UPROPERTY(EditAnywhere, Category = "Physics")
    FVehicleEngineConfig Engine;
    UPROPERTY(EditAnywhere, Category = "Physics")
    FVehicleTransmissionConfig Transmission;
    UPROPERTY(EditAnywhere, Category = "Physics")
    FVehicleDifferentialConfig Differential;
    UPROPERTY(EditAnywhere, Category = "Physics")
    FVehicleSteeringConfig Steering;
    UPROPERTY(EditAnywhere, Category = "Physics", meta = (ClampMin = "1"))
    float MassKg = 1500.f;

    // ==============================
    // GAMEPLAY
    // ==============================

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    FText DisplayName = FText::FromString(TEXT("Vehicle"));

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Engine")
    bool bAutoStartOnEnter = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Engine",
              meta = (ClampMin = "0"))
    float MaxSpeedKmh = 180.0f;

    // ==============================
    // FUEL
    // ==============================

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Fuel",
              meta = (ClampMin = "1"))
    float FuelCapacityLiters = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Fuel",
              meta = (ClampMin = "0"))
    float InitialFuelLiters = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Fuel",
              meta = (ClampMin = "0"))
    float IdleConsumptionLitersPerHour = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Fuel",
              meta = (ClampMin = "0"))
    float ConsumptionLitersPer100Km = 9.0f;

    // ==============================
    // AUDIO
    // ==============================

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Audio")
    TObjectPtr<USoundBase> HornSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Audio",
              meta = (ClampMin = "0.0"))
    float HornVolume = 1.0f;

    // ==============================
    // LIGHTS
    // ==============================

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Lights")
    bool bHasHeadlights = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Lights")
    float HeadlightIntensity = 8000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Lights")
    float HeadlightAttenuationRadius = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Lights")
    float HeadlightInnerConeAngle = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Lights")
    float HeadlightOuterConeAngle = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Lights")
    float TailLightIntensity = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Lights")
    float BrakeLightIntensity = 2500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Lights")
    float ReverseLightIntensity = 2000.0f;
};

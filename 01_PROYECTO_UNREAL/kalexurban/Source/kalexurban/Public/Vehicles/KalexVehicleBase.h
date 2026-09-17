#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Interaction/KalexInteractable.h"
#include "KalexVehicleBase.generated.h"

class UKalexVehicleSeatComponent;
class USpringArmComponent;
class UCameraComponent;
class ACharacter;
class UKalexVehicleDefinition;
class UStaticMeshComponent;
class UAudioComponent;
class USpotLightComponent;
class UPointLightComponent;
class UMaterialInstanceDynamic;
class UKalexVehicleDamageComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class USceneComponent;

UENUM(BlueprintType)
enum class EKalexEngineState : uint8
{
    Off,
    Starting,
    Running
};

UENUM(BlueprintType)
enum class EKalexSeatState : uint8
{
    Empty,
    Entering,
    Occupied,
    Exiting
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FKalexSeatChanged,
    EKalexSeatState,
    State);

UCLASS()
class KALEXURBAN_API AKalexVehicleBase
    : public AWheeledVehiclePawn,
      public IKalexInteractable
{
    GENERATED_BODY()

public:
    AKalexVehicleBase(const FObjectInitializer &ObjectInitializer);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle")
    TObjectPtr<UKalexVehicleDefinition> Definition;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
    TObjectPtr<UKalexVehicleSeatComponent> DriverSeat;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
    TObjectPtr<USpringArmComponent> CameraArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Damage")
    TObjectPtr<UKalexVehicleDamageComponent> DamageComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Damage")
    TObjectPtr<USceneComponent> EngineDamagePoint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Damage")
    TObjectPtr<UNiagaraComponent> DamageSmoke;

    UFUNCTION()
    void HandleDamageStateChanged(EKalexVehicleDamageState NewState);

    /*
     * Para templates como SportsCar donde el SKM contiene
     * el skeleton/Chaos pero la carrocería visible es StaticMesh.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Visual")
    TObjectPtr<UStaticMeshComponent> VisualBody;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Visual")
    TObjectPtr<UStaticMeshComponent> VisualLights;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Audio")
    TObjectPtr<UAudioComponent> HornAudioComponent;

    // ==============================
    // LIGHT COMPONENTS
    // ==============================

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Lights")
    TObjectPtr<USpotLightComponent> HeadlightLeft;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Lights")
    TObjectPtr<USpotLightComponent> HeadlightRight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Lights")
    TObjectPtr<UPointLightComponent> BrakeLightLeft;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Lights")
    TObjectPtr<UPointLightComponent> BrakeLightRight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Lights")
    TObjectPtr<UPointLightComponent> ReverseLightLeft;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Lights")
    TObjectPtr<UPointLightComponent> ReverseLightRight;

    // ==============================
    // LIGHTS
    // ==============================

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Lights")
    void ToggleHeadlights();

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Lights")
    void SetHeadlights(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Vehicle|Lights")
    bool AreHeadlightsOn() const
    {
        return bHeadlightsOn;
    }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Lights")
    bool AreBrakeLightsOn() const
    {
        return bBrakeLightsOn;
    }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Lights")
    bool AreReverseLightsOn() const
    {
        return bReverseLightsOn;
    }

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Vehicle")
    EKalexSeatState SeatState = EKalexSeatState::Empty;

    UPROPERTY(BlueprintAssignable, Category = "Vehicle")
    FKalexSeatChanged OnSeatChanged;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Vehicle",
        meta = (ClampMin = "0"))
    float MaxEntryExitSpeed = 100.0f;

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    bool TryEnter(ACharacter *Character);

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    bool TryExit();

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    void ApplyDriveInput(
        float Throttle,
        float Brake,
        float Steering,
        bool Handbrake);

    UFUNCTION(BlueprintPure, Category = "Vehicle")
    float GetForwardSpeedKmh() const;

    virtual bool CanInteract_Implementation(
        APawn *Interactor) const override;

    virtual FText GetInteractionText_Implementation()
        const override;

    virtual bool Interact_Implementation(
        APawn *Interactor) override;

    // ==============================
    // ENGINE
    // ==============================

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Vehicle|Engine")
    EKalexEngineState EngineState = EKalexEngineState::Off;

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Engine")
    bool StartEngine();

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Engine")
    void StopEngine();

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Engine")
    void ToggleEngine();

    UFUNCTION(BlueprintPure, Category = "Vehicle|Engine")
    bool IsEngineRunning() const
    {
        return EngineState == EKalexEngineState::Running;
    }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Engine")
    float GetEngineRPM() const;

    UFUNCTION(BlueprintPure, Category = "Vehicle|Engine")
    int32 GetCurrentGear() const;

    UFUNCTION(BlueprintPure, Category = "Vehicle|Engine")
    FText GetCurrentGearText() const;

    // ==============================
    // FUEL
    // ==============================

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Vehicle|Fuel")
    float CurrentFuelLiters = 0.0f;

    UFUNCTION(BlueprintPure, Category = "Vehicle|Fuel")
    float GetFuelLiters() const
    {
        return CurrentFuelLiters;
    }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Fuel")
    float GetFuelPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Fuel")
    void FillFuel();

    UFUNCTION(BlueprintPure, Category = "Vehicle|Fuel")
    float GetFuelCapacity() const;

    // ==============================
    // HORN
    // ==============================

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Audio")
    void StartHorn();

    UFUNCTION(BlueprintCallable, Category = "Vehicle|Audio")
    void StopHorn();

    UFUNCTION(BlueprintPure, Category = "Vehicle|Audio")
    bool IsHornActive() const;

protected:
    virtual void OnConstruction(
        const FTransform &Transform) override;

    virtual void EndPlay(
        const EEndPlayReason::Type Reason) override;

    virtual void UnPossessed() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION()
    void OnVehicleHit(
        UPrimitiveComponent *HitComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComponent,
        FVector NormalImpulse,
        const FHitResult &Hit);

private:
    UPROPERTY()
    TObjectPtr<ACharacter> Occupant;

    TWeakObjectPtr<AController> DriverController;

    FTransform EntryTransform;

    bool bWasHidden = false;
    bool bHadCollision = true;

    void RestoreCharacter(const FVector &Location);
    void SetSeatState(EKalexSeatState State);

    bool bHeadlightsOn = false;
    bool bBrakeLightsOn = false;
    bool bReverseLightsOn = false;

    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> HeadlightMaterial;

    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> BrakeLightMaterial;

    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> ReverseLightMaterial;

    void InitializeLightMaterials();
    void UpdateLightMaterials();

    void UpdateAutomaticLights(float BrakeInput);
};
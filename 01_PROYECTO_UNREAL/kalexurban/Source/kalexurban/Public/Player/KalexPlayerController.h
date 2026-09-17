#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KalexPlayerController.generated.h"

class UKalexInteractionComponent;
class UInputMappingContext;

class UKalexHUDWidget;
class AKalexVehicleBase;

UCLASS()
class KALEXURBAN_API AKalexPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AKalexPlayerController();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kalex")
    TObjectPtr<UKalexInteractionComponent> Interaction;

    UPROPERTY(EditDefaultsOnly, Category = "Kalex|Input")
    TArray<TObjectPtr<UInputMappingContext>> WalkingContexts;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kalex|UI")
    TSubclassOf<UKalexHUDWidget> HUDWidgetClass;

    UPROPERTY()
    TObjectPtr<UKalexHUDWidget> HUDWidget;

    // Permite desactivar el input de teclado del vehículo.
    // Los Automation Tests de Kalex también utilizan esta propiedad.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kalex|Vehicle")
    bool bKeyboardVehicleInput = true;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void PlayerTick(float DeltaSeconds) override;

private:
    void Interact();
    void VehicleAction();
    void VehicleLookYaw(float Value);
    void VehicleLookPitch(float Value);
    void RefuelVehicle();
    // ==============================
    // VEHICLE CAMERA
    // ==============================

    float LastVehicleCameraInputTime = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Kalex|Vehicle|Camera")
    float VehicleCameraRecenterDelay = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Kalex|Vehicle|Camera")
    float VehicleCameraRecenterSpeed = 3.0f;

    UPROPERTY(EditAnywhere, Category = "Kalex|Vehicle|Camera")
    float VehicleCameraMinRecenterSpeedKmh = 5.0f;

    void UpdateVehicleCamera(
        AKalexVehicleBase *Vehicle,
        float DeltaSeconds);

    void VehicleHornPressed();
    void VehicleHornReleased();
    void ToggleVehicleLights();
};
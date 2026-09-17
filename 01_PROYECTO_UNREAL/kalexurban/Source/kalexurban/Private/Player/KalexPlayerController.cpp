#include "Player/KalexPlayerController.h"

#include "Interaction/KalexInteractionComponent.h"
#include "Vehicles/KalexVehicleBase.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputCoreTypes.h"
#include "UObject/ConstructorHelpers.h"
#include "World/Fuel/KalexFuelStation.h"

#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "UI/KalexHUDWidget.h"

AKalexPlayerController::AKalexPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;

    Interaction =
        CreateDefaultSubobject<UKalexInteractionComponent>(TEXT("Interaction"));

    static ConstructorHelpers::FObjectFinder<UInputMappingContext> Move(
        TEXT("/Game/Input/IMC_Default"));

    static ConstructorHelpers::FObjectFinder<UInputMappingContext> Look(
        TEXT("/Game/Input/IMC_MouseLook"));

    if (Move.Succeeded())
    {
        WalkingContexts.Add(Move.Object);
    }

    if (Look.Succeeded())
    {
        WalkingContexts.Add(Look.Object);
    }
}

void AKalexPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // ==========================
    // INPUT
    // ==========================

    if (ULocalPlayer *LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem *InputSubsystem =
                LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            for (UInputMappingContext *Context : WalkingContexts)
            {
                if (Context)
                {
                    InputSubsystem->AddMappingContext(Context, 0);
                }
            }
        }
    }

    // ==========================
    // HUD
    // ==========================

    if (IsLocalController() && HUDWidgetClass)
    {
        HUDWidget =
            CreateWidget<UKalexHUDWidget>(
                this,
                HUDWidgetClass);

        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }
}

void AKalexPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    check(InputComponent);

    // E = interacción general
    InputComponent->BindKey(
        EKeys::E,
        IE_Pressed,
        this,
        &AKalexPlayerController::Interact);

    // F = entrar / salir de vehículo
    InputComponent->BindKey(
        EKeys::F,
        IE_Pressed,
        this,
        &AKalexPlayerController::VehicleAction);

    InputComponent->BindAxisKey(
        EKeys::MouseX,
        this,
        &AKalexPlayerController::VehicleLookYaw);

    InputComponent->BindAxisKey(
        EKeys::MouseY,
        this,
        &AKalexPlayerController::VehicleLookPitch);

    InputComponent->BindKey(
        EKeys::Enter,
        IE_Pressed,
        this,
        &AKalexPlayerController::RefuelVehicle);

    // H = claxon
    InputComponent->BindKey(
        EKeys::H,
        IE_Pressed,
        this,
        &AKalexPlayerController::VehicleHornPressed);

    InputComponent->BindKey(
        EKeys::H,
        IE_Released,
        this,
        &AKalexPlayerController::VehicleHornReleased);

    // L = luces del vehículo
    InputComponent->BindKey(
        EKeys::L,
        IE_Pressed,
        this,
        &AKalexPlayerController::ToggleVehicleLights);
}

void AKalexPlayerController::Interact()
{
    // Dentro de vehículo:
    // E = encender / apagar motor
    if (AKalexVehicleBase *Vehicle =
            Cast<AKalexVehicleBase>(GetPawn()))
    {
        Vehicle->ToggleEngine();
        return;
    }

    // Caminando:
    // E = interacción general
    if (Interaction)
    {
        Interaction->TryInteract();
    }
}

void AKalexPlayerController::VehicleAction()
{
    // Ya estamos dentro de un vehículo -> salir
    if (AKalexVehicleBase *Vehicle =
            Cast<AKalexVehicleBase>(GetPawn()))
    {
        Vehicle->TryExit();
        return;
    }

    // Estamos caminando -> intentar entrar
    if (Interaction)
    {
        Interaction->TryInteract();
    }
}

void AKalexPlayerController::VehicleLookYaw(float Value)
{
    if (!Cast<AKalexVehicleBase>(GetPawn()))
    {
        return;
    }

    if (FMath::IsNearlyZero(Value))
    {
        return;
    }

    LastVehicleCameraInputTime = GetWorld()->GetTimeSeconds();

    AddYawInput(Value);
}

void AKalexPlayerController::VehicleLookPitch(float Value)
{
    if (!Cast<AKalexVehicleBase>(GetPawn()))
    {
        return;
    }

    if (FMath::IsNearlyZero(Value))
    {
        return;
    }

    LastVehicleCameraInputTime = GetWorld()->GetTimeSeconds();

    AddPitchInput(-Value);
}

void AKalexPlayerController::PlayerTick(float DeltaSeconds)
{
    Super::PlayerTick(DeltaSeconds);

    if (!bKeyboardVehicleInput)
    {
        return;
    }

    AKalexVehicleBase *Vehicle =
        Cast<AKalexVehicleBase>(GetPawn());

    if (!Vehicle)
    {
        return;
    }

    float Throttle = 0.0f;
    float Brake = 0.0f;
    float Steering = 0.0f;

    const bool bForward = IsInputKeyDown(EKeys::W);
    const bool bBackward = IsInputKeyDown(EKeys::S);
    const bool bLeft = IsInputKeyDown(EKeys::A);
    const bool bRight = IsInputKeyDown(EKeys::D);
    const bool bHandbrake = IsInputKeyDown(EKeys::SpaceBar);

    const float ForwardSpeed = Vehicle->GetForwardSpeedKmh();

    if (bForward && !bBackward)
    {
        Throttle = 1.0f;
    }
    else if (bBackward && !bForward)
    {
        Brake = 1.0f;
    }

    Steering =
        (bRight ? 1.0f : 0.0f) -
        (bLeft ? 1.0f : 0.0f);

    Vehicle->ApplyDriveInput(
        Throttle,
        Brake,
        Steering,
        bHandbrake);

    UpdateVehicleCamera(
        Vehicle,
        DeltaSeconds);
}

void AKalexPlayerController::RefuelVehicle()
{
    AKalexVehicleBase *Vehicle =
        Cast<AKalexVehicleBase>(GetPawn());

    if (!Vehicle)
    {
        return;
    }

    AKalexFuelStation *CurrentStation = nullptr;

    for (TActorIterator<AKalexFuelStation> It(GetWorld()); It; ++It)
    {
        if (It->GetVehicleInZone() == Vehicle)
        {
            CurrentStation = *It;
            break;
        }
    }

    // No estamos en una gasolinera.
    if (!CurrentStation)
    {
        return;
    }

    if (Vehicle->IsEngineRunning())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                3.0f,
                FColor::Yellow,
                TEXT("GASOLINERA: Apaga el motor [E]"));
        }

        return;
    }

    const float Before =
        Vehicle->GetFuelLiters();

    const float Capacity =
        Vehicle->GetFuelCapacity();

    if (Before >= Capacity - KINDA_SMALL_NUMBER)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                3.0f,
                FColor::Green,
                TEXT("GASOLINERA: Tanque lleno"));
        }

        return;
    }

    const float LitersNeeded =
        CurrentStation->GetLitersNeeded();

    const float Cost =
        CurrentStation->GetRefuelCost();

    if (!CurrentStation->TryRefuel())
    {
        return;
    }

    if (GEngine)
    {
        const FString Message = FString::Printf(
            TEXT(
                "REPOSTAJE COMPLETADO | %.1f L | %.1f / %.1f L | S/ %.2f"),
            LitersNeeded,
            Vehicle->GetFuelLiters(),
            Capacity,
            Cost);

        GEngine->AddOnScreenDebugMessage(
            -1,
            4.0f,
            FColor::Green,
            Message);
    }
}

void AKalexPlayerController::UpdateVehicleCamera(
    AKalexVehicleBase *Vehicle,
    float DeltaSeconds)
{
    if (!Vehicle || !GetWorld())
    {
        return;
    }

    // No recentrar estando prácticamente detenido.
    if (FMath::Abs(Vehicle->GetForwardSpeedKmh()) <
        VehicleCameraMinRecenterSpeedKmh)
    {
        return;
    }

    const float CurrentTime =
        GetWorld()->GetTimeSeconds();

    // El jugador todavía está controlando/mirando la cámara.
    if ((CurrentTime - LastVehicleCameraInputTime) <
        VehicleCameraRecenterDelay)
    {
        return;
    }

    FRotator CurrentRotation =
        GetControlRotation();

    // La cámara vuelve a mirar en la dirección del vehículo.
    const float TargetYaw =
        Vehicle->GetActorRotation().Yaw;

    const float NewYaw =
        FMath::FInterpTo(
            CurrentRotation.Yaw,
            TargetYaw,
            DeltaSeconds,
            VehicleCameraRecenterSpeed);

    CurrentRotation.Yaw = NewYaw;

    SetControlRotation(CurrentRotation);
}

void AKalexPlayerController::VehicleHornPressed()
{
    if (AKalexVehicleBase *Vehicle =
            Cast<AKalexVehicleBase>(GetPawn()))
    {
        Vehicle->StartHorn();
    }
}

void AKalexPlayerController::VehicleHornReleased()
{
    if (AKalexVehicleBase *Vehicle =
            Cast<AKalexVehicleBase>(GetPawn()))
    {
        Vehicle->StopHorn();
    }
}

void AKalexPlayerController::ToggleVehicleLights()
{
    if (AKalexVehicleBase* Vehicle =
            Cast<AKalexVehicleBase>(GetPawn()))
    {
        Vehicle->ToggleHeadlights();
    }
}

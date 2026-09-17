#include "Vehicles/KalexVehicleBase.h"

#include "Vehicles/KalexVehicleSeatComponent.h"
#include "Vehicles/KalexVehicleDefinition.h"

#include "ChaosVehicleMovementComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Camera/CameraComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Vehicles/KalexVehicleDamageComponent.h"

#include "NiagaraComponent.h"

AKalexVehicleBase::AKalexVehicleBase(
    const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    DriverSeat =
        CreateDefaultSubobject<UKalexVehicleSeatComponent>(
            TEXT("DriverSeat"));

    DriverSeat->SetupAttachment(GetMesh());

    CameraArm =
        CreateDefaultSubobject<USpringArmComponent>(
            TEXT("CameraArm"));

    CameraArm->SetupAttachment(GetMesh());

    CameraArm->TargetArmLength = 600.0f;
    CameraArm->SetRelativeLocation(
        FVector(0.0f, 0.0f, 150.0f));

    CameraArm->SetRelativeRotation(
        FRotator(-15.0f, 0.0f, 0.0f));

    CameraArm->bEnableCameraLag = true;
    CameraArm->CameraLagSpeed = 8.0f;

    Camera =
        CreateDefaultSubobject<UCameraComponent>(
            TEXT("Camera"));

    Camera->SetupAttachment(
        CameraArm,
        USpringArmComponent::SocketName);

    HornAudioComponent =
        CreateDefaultSubobject<UAudioComponent>(
            TEXT("HornAudioComponent"));

    HornAudioComponent->SetupAttachment(GetMesh());

    HornAudioComponent->bAutoActivate = false;

    HeadlightLeft =
        CreateDefaultSubobject<USpotLightComponent>(
            TEXT("HeadlightLeft"));

    HeadlightLeft->SetupAttachment(GetMesh());
    HeadlightLeft->SetVisibility(false);

    HeadlightRight =
        CreateDefaultSubobject<USpotLightComponent>(
            TEXT("HeadlightRight"));

    HeadlightRight->SetupAttachment(GetMesh());
    HeadlightRight->SetVisibility(false);

    BrakeLightLeft =
        CreateDefaultSubobject<UPointLightComponent>(
            TEXT("BrakeLightLeft"));

    BrakeLightLeft->SetupAttachment(GetMesh());
    BrakeLightLeft->SetVisibility(false);

    BrakeLightRight =
        CreateDefaultSubobject<UPointLightComponent>(
            TEXT("BrakeLightRight"));

    BrakeLightRight->SetupAttachment(GetMesh());
    BrakeLightRight->SetVisibility(false);

    ReverseLightLeft =
        CreateDefaultSubobject<UPointLightComponent>(
            TEXT("ReverseLightLeft"));

    ReverseLightLeft->SetupAttachment(GetMesh());
    ReverseLightLeft->SetVisibility(false);

    ReverseLightRight =
        CreateDefaultSubobject<UPointLightComponent>(
            TEXT("ReverseLightRight"));

    ReverseLightRight->SetupAttachment(GetMesh());
    ReverseLightRight->SetVisibility(false);

    DamageComponent =
        CreateDefaultSubobject<UKalexVehicleDamageComponent>(
            TEXT("DamageComponent"));

    EngineDamagePoint = CreateDefaultSubobject<USceneComponent>(
        TEXT("EngineDamagePoint"));

    EngineDamagePoint->SetupAttachment(GetMesh());

    DamageSmoke = CreateDefaultSubobject<UNiagaraComponent>(
        TEXT("DamageSmoke"));

    DamageSmoke->SetupAttachment(EngineDamagePoint);
    DamageSmoke->bAutoActivate = false;

    /*
     * Carrocería visual independiente.
     *
     * No necesita sockets/bones.
     * Sigue el transform general del vehículo.
     */
    VisualBody =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("VisualBody"));

    VisualBody->SetupAttachment(GetMesh());

    VisualBody->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly);

    VisualBody->SetCollisionResponseToAllChannels(
        ECR_Ignore);

    /*
     * Necesario para que InteractionComponent pueda
     * encontrar el vehículo mirando la carrocería.
     */
    VisualBody->SetCollisionResponseToChannel(
        ECC_Visibility,
        ECR_Block);

    VisualLights =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("VisualLights"));

    VisualLights->SetupAttachment(GetMesh());

    VisualLights->SetCollisionEnabled(
        ECollisionEnabled::NoCollision);

    VisualLights->SetCastShadow(false);

    GetMesh()->SetCollisionProfileName(TEXT("Vehicle"));
}

void AKalexVehicleBase::BeginPlay()
{
    Super::BeginPlay();

    if (Definition)
    {
        InitializeLightMaterials();
        CurrentFuelLiters = FMath::Clamp(
            Definition->InitialFuelLiters,
            0.0f,
            Definition->FuelCapacityLiters);
    }

    if (USkeletalMeshComponent *VehicleMesh = GetMesh())
    {
        VehicleMesh->SetNotifyRigidBodyCollision(true);

        VehicleMesh->OnComponentHit.AddDynamic(
            this,
            &AKalexVehicleBase::OnVehicleHit);
    }

    if (DamageComponent)
    {
        DamageComponent->OnDamageStateChanged.AddDynamic(
            this,
            &AKalexVehicleBase::HandleDamageStateChanged);
    }

    EngineState = EKalexEngineState::Off;
}

bool AKalexVehicleBase::StartEngine()
{
    if (EngineState == EKalexEngineState::Running)
    {
        return true;
    }

    if (!Definition)
    {
        return false;
    }

    if (CurrentFuelLiters <= KINDA_SMALL_NUMBER)
    {
        return false;
    }

    EngineState = EKalexEngineState::Running;

    return true;
}

void AKalexVehicleBase::StopEngine()
{
    if (EngineState == EKalexEngineState::Off)
    {
        return;
    }

    EngineState = EKalexEngineState::Off;

    ApplyDriveInput(
        0.0f,
        1.0f,
        0.0f,
        true);
}

void AKalexVehicleBase::ToggleEngine()
{
    if (IsEngineRunning())
    {
        StopEngine();
    }
    else
    {
        StartEngine();
    }
}

void AKalexVehicleBase::OnConstruction(
    const FTransform &Transform)
{
    Super::OnConstruction(Transform);

    if (!Definition)
    {
        return;
    }

    if (!Definition->Mesh)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("KalexVehicle %s: Definition has no SkeletalMesh"),
            *GetName());

        return;
    }

    UChaosWheeledVehicleMovementComponent *Movement =
        Cast<UChaosWheeledVehicleMovementComponent>(
            GetVehicleMovementComponent());

    if (!Movement)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("KalexVehicle %s: invalid Chaos movement component"),
            *GetName());

        return;
    }

    GetMesh()->SetSkeletalMesh(Definition->Mesh);

    if (VisualLights)
    {
        VisualLights->SetStaticMesh(
            Definition->LightsMesh);
    }

    if (Definition->AnimationClass)
    {
        GetMesh()->SetAnimationMode(
            EAnimationMode::AnimationBlueprint);

        GetMesh()->SetAnimInstanceClass(
            Definition->AnimationClass);
    }

    Movement->WheelSetups =
        Definition->Wheels;

    Movement->EngineSetup =
        Definition->Engine;

    Movement->TransmissionSetup =
        Definition->Transmission;

    Movement->DifferentialSetup =
        Definition->Differential;

    Movement->SteeringSetup =
        Definition->Steering;

    Movement->Mass =
        Definition->MassKg;

    if (HeadlightLeft && HeadlightRight)
    {
        HeadlightLeft->SetIntensity(
            Definition->HeadlightIntensity);

        HeadlightRight->SetIntensity(
            Definition->HeadlightIntensity);

        HeadlightLeft->SetAttenuationRadius(
            Definition->HeadlightAttenuationRadius);

        HeadlightRight->SetAttenuationRadius(
            Definition->HeadlightAttenuationRadius);

        HeadlightLeft->SetInnerConeAngle(
            Definition->HeadlightInnerConeAngle);

        HeadlightRight->SetInnerConeAngle(
            Definition->HeadlightInnerConeAngle);

        HeadlightLeft->SetOuterConeAngle(
            Definition->HeadlightOuterConeAngle);

        HeadlightRight->SetOuterConeAngle(
            Definition->HeadlightOuterConeAngle);
    }

    if (BrakeLightLeft && BrakeLightRight)
    {
        BrakeLightLeft->SetIntensity(
            Definition->BrakeLightIntensity);

        BrakeLightRight->SetIntensity(
            Definition->BrakeLightIntensity);
    }

    if (ReverseLightLeft && ReverseLightRight)
    {
        ReverseLightLeft->SetIntensity(
            Definition->ReverseLightIntensity);

        ReverseLightRight->SetIntensity(
            Definition->ReverseLightIntensity);
    }

    GetMesh()->SetSimulatePhysics(true);

    Movement->RecreatePhysicsState();
}

void AKalexVehicleBase::SetSeatState(
    EKalexSeatState State)
{
    SeatState = State;
    OnSeatChanged.Broadcast(State);
}

bool AKalexVehicleBase::CanInteract_Implementation(
    APawn *Interactor) const
{
    const ACharacter *Character =
        Cast<ACharacter>(Interactor);

    if (!Character)
    {
        return false;
    }

    if (!Interactor->GetController())
    {
        return false;
    }

    if (SeatState != EKalexSeatState::Empty)
    {
        return false;
    }

    if (GetController())
    {
        return false;
    }

    if (FMath::Abs(GetForwardSpeedKmh()) > 2.0f)
    {
        return false;
    }

    if (!DriverSeat)
    {
        return false;
    }

    const float DistanceSquared =
        FVector::DistSquared(
            Interactor->GetActorLocation(),
            DriverSeat->GetComponentLocation());

    return DistanceSquared <=
           FMath::Square(DriverSeat->EntryDistance);
}

FText AKalexVehicleBase::GetInteractionText_Implementation()
    const
{
    return NSLOCTEXT(
        "Kalex",
        "EnterVehicle",
        "Entrar al vehiculo");
}

bool AKalexVehicleBase::Interact_Implementation(
    APawn *Interactor)
{
    return TryEnter(
        Cast<ACharacter>(Interactor));
}

bool AKalexVehicleBase::TryEnter(
    ACharacter *Character)
{
    if (!Character)
    {
        return false;
    }

    if (!CanInteract_Implementation(Character))
    {
        return false;
    }

    AController *EnteringController =
        Character->GetController();

    if (!EnteringController)
    {
        return false;
    }

    SetSeatState(EKalexSeatState::Entering);

    Occupant = Character;
    DriverController = EnteringController;

    EntryTransform =
        Character->GetActorTransform();

    bWasHidden =
        Character->IsHidden();

    bHadCollision =
        Character->GetActorEnableCollision();

    Character
        ->GetCharacterMovement()
        ->StopMovementImmediately();

    Character
        ->GetCharacterMovement()
        ->DisableMovement();

    Character->SetActorEnableCollision(false);
    Character->SetActorHiddenInGame(true);

    Character->AttachToComponent(
        DriverSeat,
        FAttachmentTransformRules::
            SnapToTargetNotIncludingScale);

    EnteringController->Possess(this);

    if (GetController() != EnteringController)
    {
        RestoreCharacter(
            EntryTransform.GetLocation());

        return false;
    }

    SetSeatState(EKalexSeatState::Occupied);
    if (Definition && Definition->bAutoStartOnEnter)
    {
        StartEngine();
    }

    ApplyDriveInput(
        0.0f,
        1.0f,
        0.0f,
        true);

    return true;
}

bool AKalexVehicleBase::TryExit()
{
    if (
        SeatState != EKalexSeatState::Occupied ||
        !IsValid(Occupant) ||
        !DriverController.IsValid())
    {
        return false;
    }

    if (FMath::Abs(GetForwardSpeedKmh()) > 2.0f)
    {
        return false;
    }

    FVector ExitLocation;

    if (!DriverSeat->FindSafeExit(
            Occupant,
            ExitLocation))
    {
        return false;
    }

    SetSeatState(EKalexSeatState::Exiting);

    RestoreCharacter(ExitLocation);

    return true;
}

void AKalexVehicleBase::RestoreCharacter(
    const FVector &Location)
{
    ACharacter *Character = Occupant;
    AController *RestoringController =
        DriverController.Get();

    Occupant = nullptr;
    DriverController.Reset();

    ApplyDriveInput(
        0.0f,
        1.0f,
        0.0f,
        true);

    if (!IsValid(Character))
    {
        SetSeatState(EKalexSeatState::Empty);
        return;
    }

    Character->DetachFromActor(
        FDetachmentTransformRules::
            KeepWorldTransform);

    Character->SetActorLocationAndRotation(
        Location,
        FRotator(
            0.0f,
            GetActorRotation().Yaw,
            0.0f),
        false,
        nullptr,
        ETeleportType::TeleportPhysics);

    Character->SetActorHiddenInGame(
        bWasHidden);

    Character->SetActorEnableCollision(
        bHadCollision);

    Character
        ->GetCharacterMovement()
        ->SetMovementMode(MOVE_Walking);

    if (
        RestoringController &&
        (!RestoringController->GetPawn() ||
         RestoringController->GetPawn() == this))
    {
        RestoringController->Possess(Character);
    }

    SetSeatState(EKalexSeatState::Empty);
}

void AKalexVehicleBase::ApplyDriveInput(
    float Throttle,
    float Brake,
    float Steering,
    bool Handbrake)
{
    UChaosVehicleMovementComponent *Movement =
        GetVehicleMovementComponent();

    if (!Movement)
    {
        return;
    }

    if (!IsEngineRunning())
    {
        Movement->SetThrottleInput(0.0f);
        Movement->SetBrakeInput(1.0f);
        Movement->SetSteeringInput(0.0f);
        Movement->SetHandbrakeInput(true);
        return;
    }

    Movement->SetThrottleInput(
        FMath::Clamp(Throttle, 0.0f, 1.0f));

    Movement->SetBrakeInput(
        FMath::Clamp(Brake, 0.0f, 1.0f));

    Movement->SetSteeringInput(
        FMath::Clamp(Steering, -1.0f, 1.0f));

    Movement->SetHandbrakeInput(Handbrake);

    UpdateAutomaticLights(Brake);
}

float AKalexVehicleBase::GetEngineRPM() const
{
    const UChaosWheeledVehicleMovementComponent *Movement =
        Cast<UChaosWheeledVehicleMovementComponent>(
            GetVehicleMovementComponent());

    if (!Movement || !IsEngineRunning())
    {
        return 0.0f;
    }

    return Movement->GetEngineRotationSpeed();
}

float AKalexVehicleBase::GetFuelCapacity() const
{
    return Definition
               ? Definition->FuelCapacityLiters
               : 0.0f;
}

float AKalexVehicleBase::GetFuelPercent() const
{
    const float Capacity = GetFuelCapacity();

    if (Capacity <= 0.0f)
    {
        return 0.0f;
    }

    return FMath::Clamp(
        CurrentFuelLiters / Capacity,
        0.0f,
        1.0f);
}

void AKalexVehicleBase::FillFuel()
{
    if (!Definition)
    {
        return;
    }

    CurrentFuelLiters =
        Definition->FuelCapacityLiters;
}

void AKalexVehicleBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!Definition || !IsEngineRunning())
    {
        return;
    }

    // Consumo del motor simplemente por estar encendido.
    const float IdleConsumption =
        Definition->IdleConsumptionLitersPerHour / 3600.0f * DeltaSeconds;

    // Consumo asociado a distancia recorrida.
    const float SpeedKmh =
        FMath::Abs(GetForwardSpeedKmh());

    const float DistanceKm =
        SpeedKmh * (DeltaSeconds / 3600.0f);

    const float DrivingConsumption =
        DistanceKm * (Definition->ConsumptionLitersPer100Km / 100.0f);

    CurrentFuelLiters = FMath::Max(
        0.0f,
        CurrentFuelLiters - IdleConsumption - DrivingConsumption);

    if (CurrentFuelLiters <= KINDA_SMALL_NUMBER)
    {
        CurrentFuelLiters = 0.0f;
        StopEngine();
    }
}

float AKalexVehicleBase::GetForwardSpeedKmh() const
{
    const UChaosVehicleMovementComponent *Movement =
        GetVehicleMovementComponent();

    if (!Movement)
    {
        return 0.0f;
    }

    /*
     * Chaos devuelve cm/s.
     * cm/s * 0.036 = km/h.
     */
    return Movement->GetForwardSpeed() * 0.036f;
}

void AKalexVehicleBase::UnPossessed()
{
    Super::UnPossessed();

    ApplyDriveInput(
        0.0f,
        1.0f,
        0.0f,
        true);
}

void AKalexVehicleBase::EndPlay(
    const EEndPlayReason::Type Reason)
{
    if (
        Reason == EEndPlayReason::Destroyed &&
        Occupant)
    {
        FVector ExitLocation =
            EntryTransform.GetLocation();

        if (DriverSeat)
        {
            DriverSeat->FindSafeExit(
                Occupant,
                ExitLocation);
        }

        RestoreCharacter(
            ExitLocation);
    }

    Super::EndPlay(Reason);
}
int32 AKalexVehicleBase::GetCurrentGear() const
{
    const UChaosWheeledVehicleMovementComponent *Movement =
        Cast<UChaosWheeledVehicleMovementComponent>(
            GetVehicleMovementComponent());

    if (!Movement)
    {
        return 0;
    }

    return Movement->GetCurrentGear();
}

FText AKalexVehicleBase::GetCurrentGearText() const
{
    const int32 Gear = GetCurrentGear();

    if (Gear < 0)
    {
        return FText::FromString(TEXT("R"));
    }

    if (Gear == 0)
    {
        return FText::FromString(TEXT("N"));
    }

    return FText::AsNumber(Gear);
}

void AKalexVehicleBase::StartHorn()
{
    if (!Definition ||
        !Definition->HornSound ||
        !HornAudioComponent)
    {
        return;
    }

    if (HornAudioComponent->IsPlaying())
    {
        return;
    }

    HornAudioComponent->SetSound(
        Definition->HornSound);

    HornAudioComponent->SetVolumeMultiplier(
        Definition->HornVolume);

    HornAudioComponent->Play();
}

void AKalexVehicleBase::StopHorn()
{
    if (!HornAudioComponent)
    {
        return;
    }

    if (HornAudioComponent->IsPlaying())
    {
        HornAudioComponent->Stop();
    }
}

bool AKalexVehicleBase::IsHornActive() const
{
    return HornAudioComponent &&
           HornAudioComponent->IsPlaying();
}

void AKalexVehicleBase::ToggleHeadlights()
{
    SetHeadlights(!bHeadlightsOn);
}

void AKalexVehicleBase::SetHeadlights(bool bEnabled)
{
    if (!Definition || !Definition->bHasHeadlights)
    {
        bEnabled = false;
    }

    bHeadlightsOn = bEnabled;

    if (HeadlightLeft)
    {
        HeadlightLeft->SetVisibility(bHeadlightsOn);
    }

    if (HeadlightRight)
    {
        HeadlightRight->SetVisibility(bHeadlightsOn);
    }

    // Al encender las luces generales,
    // las luces rojas traseras quedan encendidas
    // a baja intensidad.
    const float RearIntensity =
        bBrakeLightsOn
            ? Definition->BrakeLightIntensity
            : Definition->TailLightIntensity;

    if (BrakeLightLeft)
    {
        BrakeLightLeft->SetIntensity(RearIntensity);
        BrakeLightLeft->SetVisibility(
            bHeadlightsOn || bBrakeLightsOn);
    }

    if (BrakeLightRight)
    {
        BrakeLightRight->SetIntensity(RearIntensity);
        BrakeLightRight->SetVisibility(
            bHeadlightsOn || bBrakeLightsOn);
    }

    UpdateLightMaterials();
}

void AKalexVehicleBase::UpdateAutomaticLights(float BrakeInput)
{
    if (!Definition)
    {
        return;
    }

    // =====================================
    // LUCES DE FRENO / POSICIÓN
    // =====================================

    const bool bNewBrakeLights =
        BrakeInput > 0.1f;

    bBrakeLightsOn = bNewBrakeLights;

    const float RearIntensity =
        bBrakeLightsOn
            ? Definition->BrakeLightIntensity
            : Definition->TailLightIntensity;

    const bool bRearLightsVisible =
        bHeadlightsOn || bBrakeLightsOn;

    if (BrakeLightLeft)
    {
        BrakeLightLeft->SetIntensity(
            RearIntensity);

        BrakeLightLeft->SetVisibility(
            bRearLightsVisible);
    }

    if (BrakeLightRight)
    {
        BrakeLightRight->SetIntensity(
            RearIntensity);

        BrakeLightRight->SetVisibility(
            bRearLightsVisible);
    }

    // =====================================
    // LUCES DE REVERSA
    // =====================================

    const float ForwardSpeed =
        GetForwardSpeedKmh();

    /*
     * Chaos puede seleccionar R mientras todavía
     * estamos frenando hacia adelante.
     *
     * Por eso NO basta con Gear < 0.
     *
     * La luz blanca aparece únicamente cuando:
     * 1. Chaos está en reversa.
     * 2. El vehículo realmente se mueve hacia atrás.
     */
    const bool bNewReverseLights =
        GetCurrentGear() < 0 &&
        ForwardSpeed < -0.5f;

    bReverseLightsOn = bNewReverseLights;

    if (ReverseLightLeft)
    {
        ReverseLightLeft->SetVisibility(
            bReverseLightsOn);
    }

    if (ReverseLightRight)
    {
        ReverseLightRight->SetVisibility(
            bReverseLightsOn);
    }

    UpdateLightMaterials();
}

void AKalexVehicleBase::InitializeLightMaterials()
{
    if (!VisualLights)
        return;

    // Los índices corresponden a SM_SportsCar_Lights_Kalex
    HeadlightMaterial = VisualLights->CreateDynamicMaterialInstance(1);
    BrakeLightMaterial = VisualLights->CreateDynamicMaterialInstance(2);
    ReverseLightMaterial = VisualLights->CreateDynamicMaterialInstance(3);

    UpdateLightMaterials();
}

void AKalexVehicleBase::UpdateLightMaterials()
{
    // Faros delanteros
    if (HeadlightMaterial)
    {
        HeadlightMaterial->SetScalarParameterValue(
            TEXT("EmissiveIntensity"),
            bHeadlightsOn ? 15.0f : 0.0f);
    }

    // Luces traseras:
    // L encendida = rojo tenue
    // frenando = rojo fuerte
    if (BrakeLightMaterial)
    {
        float Intensity = 0.0f;

        if (bBrakeLightsOn)
            Intensity = 15.0f;
        else if (bHeadlightsOn)
            Intensity = 1.5f;

        BrakeLightMaterial->SetScalarParameterValue(
            TEXT("EmissiveIntensity"),
            Intensity);
    }

    // Reversa
    if (ReverseLightMaterial)
    {
        ReverseLightMaterial->SetScalarParameterValue(
            TEXT("EmissiveIntensity"),
            bReverseLightsOn ? 15.0f : 0.0f);
    }
}

void AKalexVehicleBase::OnVehicleHit(
    UPrimitiveComponent *HitComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComponent,
    FVector NormalImpulse,
    const FHitResult &Hit)
{
    if (!DamageComponent || DamageComponent->IsDestroyed())
    {
        return;
    }

    // Evitar impactos sin fuerza real.
    const float Impulse = NormalImpulse.Size();

    if (Impulse <= 0.0f)
    {
        return;
    }

    /*
     * Convertimos el impulso en una medida aproximada de severidad.
     * Lo importante ahora es tener un sistema jugable, no una
     * simulación estructural real.
     */
    const float MassKg = GetVehicleMovementComponent()
                             ? GetVehicleMovementComponent()->Mass
                             : 1500.0f;

    const float ImpactSeverity = Impulse / FMath::Max(MassKg, 1.0f);

    // Golpes pequeños no dañan.
    constexpr float MinimumImpact = 300.0f;

    if (ImpactSeverity < MinimumImpact)
    {
        return;
    }

    // Convertir severidad en HP.
    const float Damage = FMath::Clamp(
        (ImpactSeverity - MinimumImpact) * 0.025f,
        1.0f,
        45.0f);

    DamageComponent->ApplyVehicleDamage(Damage);

    if (DamageComponent->IsDestroyed())
    {
        StopEngine();

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("VEHICLE DESTROYED"));

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                5.0f,
                FColor::Red,
                TEXT("VEHICULO DESTRUIDO"));
        }
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("VEHICLE HIT | Severity: %.1f | Damage: %.1f | HP: %.1f/%.1f"),
        ImpactSeverity,
        Damage,
        DamageComponent->GetHealth(),
        DamageComponent->GetMaxHealth());

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::Red,
            FString::Printf(
                TEXT("CHOQUE %.0f DMG | VEHICULO %.0f HP"),
                Damage,
                DamageComponent->GetHealth()));
    }
}

void AKalexVehicleBase::HandleDamageStateChanged(
    EKalexVehicleDamageState NewState)
{
    if (!DamageSmoke)
        return;

    switch (NewState)
    {
        case EKalexVehicleDamageState::Normal:
        {
            DamageSmoke->Deactivate();
            break;
        }

        case EKalexVehicleDamageState::Damaged:
        {
            DamageSmoke->Activate(true);
            DamageSmoke->SetVariableFloat(
                FName("User.SmokeIntensity"),
                0.35f
            );
            break;
        }

        case EKalexVehicleDamageState::Critical:
        {
            DamageSmoke->Activate(true);
            DamageSmoke->SetVariableFloat(
                FName("User.SmokeIntensity"),
                0.70f
            );
            break;
        }

        case EKalexVehicleDamageState::Destroyed:
        {
            DamageSmoke->Activate(true);
            DamageSmoke->SetVariableFloat(
                FName("User.SmokeIntensity"),
                1.0f
            );
            break;
        }
    }
}

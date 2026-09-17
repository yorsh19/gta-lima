#include "UI/KalexHUDWidget.h"

#include "Vehicles/KalexVehicleBase.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Widget.h"
#include "World/Fuel/KalexFuelStation.h"
#include "EngineUtils.h"

void UKalexHUDWidget::NativeTick(
    const FGeometry &MyGeometry,
    float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    APawn *Pawn = GetOwningPlayerPawn();

    AKalexVehicleBase *Vehicle =
        Cast<AKalexVehicleBase>(Pawn);

    // ==========================
    // CAMINANDO
    // ==========================

    if (!Vehicle)
    {
        if (VehicleHUD)
        {
            VehicleHUD->SetVisibility(
                ESlateVisibility::Collapsed);
        }

        return;
    }

    // ==========================
    // CONDUCIENDO
    // ==========================

    if (VehicleHUD)
    {
        VehicleHUD->SetVisibility(
            ESlateVisibility::HitTestInvisible);
    }

    const float Speed =
        FMath::Abs(Vehicle->GetForwardSpeedKmh());

    const float RPM =
        Vehicle->GetEngineRPM();

    const float Fuel =
        Vehicle->GetFuelLiters();

    const float Capacity =
        Vehicle->GetFuelCapacity();

    const float FuelPercent =
        Vehicle->GetFuelPercent();

    if (TXT_Speed)
    {
        TXT_Speed->SetText(
            FText::AsNumber(
                FMath::RoundToInt(Speed)));
    }

    if (TXT_RPM)
    {
        TXT_RPM->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("%.0f RPM"),
                    RPM)));
    }

    if (TXT_Fuel)
    {
        TXT_Fuel->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("%.1f / %.1f L"),
                    Fuel,
                    Capacity)));
    }

    if (TXT_Gear)
    {
        TXT_Gear->SetText(
            Vehicle->GetCurrentGearText());
    }

    if (TXT_Engine)
    {
        TXT_Engine->SetText(
            Vehicle->IsEngineRunning()
                ? FText::FromString(TEXT("MOTOR ON"))
                : FText::FromString(TEXT("MOTOR OFF")));
    }

    if (PB_Fuel)
    {
        PB_Fuel->SetPercent(
            FuelPercent);
    }

    AKalexFuelStation *FuelStation = nullptr;

    for (TActorIterator<AKalexFuelStation> It(GetWorld()); It; ++It)
    {
        if (It->GetVehicleInZone() == Vehicle)
        {
            FuelStation = *It;
            break;
        }
    }

    if (!FuelStation)
    {
        if (ContextPrompt)
        {
            ContextPrompt->SetVisibility(
                ESlateVisibility::Collapsed);
        }

        return;
    }

    if (ContextPrompt)
    {
        ContextPrompt->SetVisibility(
            ESlateVisibility::HitTestInvisible);
    }

    if (Vehicle->IsEngineRunning())
    {
        if (TXT_ContextTitle)
        {
            TXT_ContextTitle->SetText(
                FText::FromString(
                    TEXT("APAGA EL MOTOR")));
        }

        if (TXT_ContextAction)
        {
            TXT_ContextAction->SetText(
                FText::FromString(TEXT("[E]")));
        }

        return;
    }

    const float LitersNeeded =
        FuelStation->GetLitersNeeded();

    if (LitersNeeded <= KINDA_SMALL_NUMBER)
    {
        if (TXT_ContextTitle)
        {
            TXT_ContextTitle->SetText(
                FText::FromString(
                    TEXT("TANQUE LLENO")));
        }

        if (TXT_ContextAction)
        {
            TXT_ContextAction->SetText(
                FText::GetEmpty());
        }

        return;
    }
    if (TXT_ContextTitle)
    {
        TXT_ContextTitle->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("REPOSTAR %.1f L"),
                    LitersNeeded)));
    }

    if (TXT_ContextAction)
    {
        TXT_ContextAction->SetText(
            FText::FromString(
                TEXT("[ENTER]")));
    }
}

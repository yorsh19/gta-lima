#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KalexHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UWidget;

UCLASS()
class KALEXURBAN_API UKalexHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeTick(
        const FGeometry &MyGeometry,
        float InDeltaTime) override;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWidget> VehicleHUD;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_Speed;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_RPM;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_Fuel;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> PB_Fuel;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_Gear;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_Engine;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWidget> ContextPrompt;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_ContextTitle;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_ContextAction;
};
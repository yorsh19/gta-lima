#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KalexGameMode.generated.h"

UCLASS()
class KALEXURBAN_API AKalexGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AKalexGameMode();
    /** Tag on APlayerStart, independent of city generation. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Kalex|Spawn")
    FName PreferredStartTag = TEXT("KalexStart");
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};

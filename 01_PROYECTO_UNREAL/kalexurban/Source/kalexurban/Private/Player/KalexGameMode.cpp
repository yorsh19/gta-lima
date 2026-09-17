#include "Player/KalexGameMode.h"
#include "Player/KalexPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"

AKalexGameMode::AKalexGameMode()
{
    PlayerControllerClass = AKalexPlayerController::StaticClass();
    static ConstructorHelpers::FClassFinder<APawn> Character(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
    if (Character.Succeeded()) DefaultPawnClass = Character.Class;
}
AActor* AKalexGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    APlayerStart* Preferred = nullptr;
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        if (It->PlayerStartTag != PreferredStartTag || PreferredStartTag.IsNone()) continue;
        if (Preferred)
        {
            UE_LOG(LogTemp, Warning, TEXT("Kalex: duplicate PlayerStart tag %s; using engine spawn selection."), *PreferredStartTag.ToString());
            return Super::ChoosePlayerStart_Implementation(Player);
        }
        Preferred = *It;
    }
    return Preferred ? Preferred : Super::ChoosePlayerStart_Implementation(Player);
}

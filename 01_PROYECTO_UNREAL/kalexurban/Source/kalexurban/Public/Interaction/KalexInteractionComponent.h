#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KalexInteractionComponent.generated.h"

UCLASS(ClassGroup=(Kalex), meta=(BlueprintSpawnableComponent))
class KALEXURBAN_API UKalexInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UKalexInteractionComponent();

    UFUNCTION(BlueprintCallable, Category="Kalex|Interaction")
    bool TryInteract();
};
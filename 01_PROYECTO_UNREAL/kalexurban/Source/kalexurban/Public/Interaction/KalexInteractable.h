#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KalexInteractable.generated.h"

UINTERFACE(BlueprintType)
class KALEXURBAN_API UKalexInteractable : public UInterface
{
    GENERATED_BODY()
};

class KALEXURBAN_API IKalexInteractable
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Kalex|Interaction")
    bool CanInteract(APawn* Interactor) const;
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Kalex|Interaction")
    FText GetInteractionText() const;
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Kalex|Interaction")
    bool Interact(APawn* Interactor);
};

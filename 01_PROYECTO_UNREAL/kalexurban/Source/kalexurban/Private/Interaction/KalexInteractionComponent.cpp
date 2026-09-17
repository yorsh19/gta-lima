#include "Interaction/KalexInteractionComponent.h"
#include "Interaction/KalexInteractable.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

UKalexInteractionComponent::UKalexInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UKalexInteractionComponent::TryInteract()
{
    APlayerController* PC = Cast<APlayerController>(GetOwner());

    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("[KALEX] Interaction: Owner no es PlayerController"));
        return false;
    }

    APawn* Pawn = PC->GetPawn();

    if (!Pawn)
    {
        UE_LOG(LogTemp, Error, TEXT("[KALEX] Interaction: PlayerController no tiene Pawn"));
        return false;
    }

    FVector Start;
    FRotator Rotation;
    PC->GetPlayerViewPoint(Start, Rotation);

    const FVector End = Start + Rotation.Vector() * 500.0f;

    FHitResult Hit;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Pawn);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

#if WITH_EDITOR
    DrawDebugLine(
        GetWorld(),
        Start,
        End,
        bHit ? FColor::Green : FColor::Red,
        false,
        3.0f,
        0,
        3.0f
    );
#endif

    if (!bHit)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                3.0f,
                FColor::Red,
                TEXT("KALEX: No hay objeto para interactuar")
            );
        }

        return false;
    }

    AActor* HitActor = Hit.GetActor();

    if (!HitActor)
    {
        return false;
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            3.0f,
            FColor::Yellow,
            FString::Printf(TEXT("KALEX HIT: %s"), *HitActor->GetName())
        );
    }

    if (!HitActor->GetClass()->ImplementsInterface(
        UKalexInteractable::StaticClass()))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                3.0f,
                FColor::Red,
                FString::Printf(
                    TEXT("KALEX: %s NO implementa KalexInteractable"),
                    *HitActor->GetName()
                )
            );
        }

        return false;
    }

    if (!IKalexInteractable::Execute_CanInteract(HitActor, Pawn))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                3.0f,
                FColor::Red,
                TEXT("KALEX: Vehiculo detectado pero CanInteract = FALSE")
            );
        }

        return false;
    }

    const bool bResult =
        IKalexInteractable::Execute_Interact(HitActor, Pawn);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            3.0f,
            bResult ? FColor::Green : FColor::Red,
            bResult
                ? TEXT("KALEX: ENTRANDO AL VEHICULO")
                : TEXT("KALEX: Interact devolvio FALSE")
        );
    }

    return bResult;
}
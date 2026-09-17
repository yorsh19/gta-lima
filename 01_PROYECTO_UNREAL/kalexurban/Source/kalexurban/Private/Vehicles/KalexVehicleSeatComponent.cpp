#include "Vehicles/KalexVehicleSeatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

UKalexVehicleSeatComponent::UKalexVehicleSeatComponent()
{
    ExitOffsets = { FVector(0,-180,0), FVector(0,180,0), FVector(-250,0,0) };
}

bool UKalexVehicleSeatComponent::FindSafeExit(ACharacter* Character, FVector& Location) const
{
    if (!IsValid(Character) || !GetWorld() || !GetOwner()) return false;
    float Radius, HalfHeight;
    Character->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(KalexExit), false, Character);
    // Keep the vehicle in clearance tests: an exit inside its chassis is unsafe.
    for (const FVector& Offset : ExitOffsets)
    {
        const FVector Candidate = GetOwner()->GetActorTransform().TransformPosition(Offset);
        FHitResult Ground;
        if (!GetWorld()->LineTraceSingleByChannel(Ground, Candidate + FVector(0,0,100),
            Candidate - FVector(0,0,250), ECC_Pawn, Params)) continue;
        if (Ground.GetActor() == GetOwner() || !Character->GetCharacterMovement()->IsWalkable(Ground)) continue;
        const FVector Center = Ground.ImpactPoint + FVector(0,0,HalfHeight + 3);
        if (GetWorld()->OverlapBlockingTestByChannel(Center, FQuat::Identity, ECC_Pawn,
            FCollisionShape::MakeCapsule(Radius, HalfHeight), Params)) continue;
        // Prevent exiting through a wall to a clear candidate on its far side.
        FCollisionQueryParams PathParams = Params;
        PathParams.AddIgnoredActor(GetOwner());
        FHitResult Obstacle;
        if (GetWorld()->SweepSingleByChannel(Obstacle, GetComponentLocation() + FVector(0,0,HalfHeight),
            Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(Radius), PathParams)) continue;
        Location = Center;
        return true;
    }
    return false;
}

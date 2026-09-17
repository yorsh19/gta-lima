#include "City/KalexUrbanZone.h"
#include "Components/BoxComponent.h"
AKalexUrbanZone::AKalexUrbanZone()
{
    PrimaryActorTick.bCanEverTick = false;
    ZoneBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBounds"));
    RootComponent = ZoneBounds;
    ZoneBounds->SetBoxExtent(FVector(2500.f, 2500.f, 100.f));
    ZoneBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ZoneBounds->ShapeColor = FColor::Cyan;
    ZoneBounds->SetLineThickness(8.f);
}

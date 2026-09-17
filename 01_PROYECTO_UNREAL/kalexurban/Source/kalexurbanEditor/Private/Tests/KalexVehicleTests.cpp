#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "Vehicles/KalexVehicleBase.h"
#include "Vehicles/KalexVehicleSeatComponent.h"
#include "Player/KalexPlayerController.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "EngineUtils.h"
#include "Interaction/KalexInteractionComponent.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKalexSeatLifecycleTest, "Kalex.Gameplay.SeatLifecycle",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FKalexSeatLifecycleTest::RunTest(const FString& Parameters)
{
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    auto Box = [World](FVector Location, FVector Extent)
    {
        AActor* Actor = World->SpawnActor<AActor>();
        UBoxComponent* Component = NewObject<UBoxComponent>(Actor);
        Actor->SetRootComponent(Component);
        Component->SetBoxExtent(Extent);
        Component->SetCollisionProfileName(TEXT("BlockAll"));
        Component->RegisterComponent();
        Actor->SetActorLocation(Location);
        return Actor;
    };
    Box(FVector(0,0,-50), FVector(3000,3000,50));
    AKalexVehicleBase* Vehicle = World->SpawnActor<AKalexVehicleBase>();
    ACharacter* Character = World->SpawnActor<ACharacter>();
    AKalexPlayerController* PC = World->SpawnActor<AKalexPlayerController>();
    Character->SetActorLocation(FVector(0,-180,100));
    PC->Possess(Character);
    for (int32 Cycle=0; Cycle<20; ++Cycle)
    {
        TestTrue(TEXT("Enter"), Vehicle->TryEnter(Character));
        TestTrue(TEXT("Vehicle possessed"), PC->GetPawn() == Vehicle);
        TestTrue(TEXT("Character hidden"), Character->IsHidden());
        TestFalse(TEXT("Double entry rejected"), Vehicle->TryEnter(Character));
        TestTrue(TEXT("Exit"), Vehicle->TryExit());
        TestTrue(TEXT("Character possessed"), PC->GetPawn() == Character);
        TestTrue(TEXT("Collision restored"), Character->GetActorEnableCollision());
        TestFalse(TEXT("Visible after exit"), Character->IsHidden());
        TestTrue(TEXT("Seat released"), Vehicle->SeatState == EKalexSeatState::Empty);
    }
    TestTrue(TEXT("Enter for blocked exit"), Vehicle->TryEnter(Character));
    TArray<AActor*> Blockers;
    for (const FVector& Offset : Vehicle->DriverSeat->ExitOffsets)
        Blockers.Add(Box(Offset + FVector(0,0,100), FVector(75,75,100)));
    TestFalse(TEXT("All exits blocked"), Vehicle->TryExit());
    TestTrue(TEXT("Keeps vehicle control when blocked"), PC->GetPawn() == Vehicle);
    for (AActor* Blocker : Blockers) Blocker->Destroy();
    TestTrue(TEXT("Exit recovers after obstruction removed"), Vehicle->TryExit());
    Character->SetActorLocation(FVector(1500,0,100));
    TestFalse(TEXT("Remote entry rejected"), Vehicle->TryEnter(Character));
    Character->SetActorLocation(FVector(0,-180,100));
    TestTrue(TEXT("Enter before destruction"), Vehicle->TryEnter(Character));
    // Editor worlds do not dispatch EndPlay; exercise external unpossession explicitly.
    PC->UnPossess();
    TestFalse(TEXT("External unpossession restores visibility"), Character->IsHidden());
    TestTrue(TEXT("External unpossession restores collision"), Character->GetActorEnableCollision());
    return true;
}
class FKalexDrivingCommand : public IAutomationLatentCommand
{
    FAutomationTestBase* Test;
    int32 Phase = 0;
    double PhaseStart = 0;
    FVector Start;
    TWeakObjectPtr<AKalexVehicleBase> Car;
    TWeakObjectPtr<ACharacter> Character;
    TWeakObjectPtr<AKalexPlayerController> PC;
public:
    explicit FKalexDrivingCommand(FAutomationTestBase* InTest) : Test(InTest) {}
    virtual bool Update() override
    {
        if (PhaseStart == 0) PhaseStart = FPlatformTime::Seconds();
        const double Elapsed = FPlatformTime::Seconds() - PhaseStart;
        UWorld* World = GEditor ? GEditor->PlayWorld : nullptr;
        if (!World)
        {
            if (Elapsed < 20) return false;
            Test->AddError(TEXT("PIE world did not start")); return true;
        }
        if (Phase == 0)
        {
            if (Elapsed < 3) return false;
            PC = Cast<AKalexPlayerController>(World->GetFirstPlayerController());
            if (!PC.IsValid()) { Test->AddError(TEXT("Kalex controller missing")); return true; }
            Character = Cast<ACharacter>(PC->GetPawn());
            if (!Character.IsValid()) { Test->AddError(TEXT("Walking character missing")); return true; }
            for (TActorIterator<AKalexVehicleBase> It(World); It; ++It)
                if (It->GetActorLocation().Y < 300) { Car = *It; break; }
            if (!Car.IsValid()) { Test->AddError(TEXT("Configured car missing")); return true; }
            Character->SetActorLocation(Car->GetActorLocation() + FVector(0,-240,90));
            FVector Eye; FRotator Rot;
            Character->GetActorEyesViewPoint(Eye, Rot);
            PC->SetControlRotation((Car->GetActorLocation()+FVector(0,0,65)-Eye).Rotation());
            if (!Test->TestTrue(TEXT("Trace interaction enters configured car"), PC->Interaction->TryInteract())) return true;
            PC->bKeyboardVehicleInput = false;
            Start = Car->GetActorLocation();
            Car->ApplyDriveInput(1,0,0,false);
            Phase = 1; PhaseStart = FPlatformTime::Seconds(); return false;
        }
        if (!Car.IsValid() || !PC.IsValid()) { Test->AddError(TEXT("Lost vehicle/controller")); return true; }
        if (Phase == 1)
        {
            Car->ApplyDriveInput(1,0,0,false);
            if (Elapsed < 3) return false;
            Test->TestTrue(TEXT("Chaos drove more than 3 metres"), FVector::Dist2D(Start,Car->GetActorLocation()) > 300);
            Test->TestTrue(TEXT("Vehicle moving"), Car->GetVelocity().Size() > Car->MaxEntryExitSpeed);
            Test->TestFalse(TEXT("Cannot exit moving vehicle"), Car->TryExit());
            Car->ApplyDriveInput(0,1,0,true);
            Phase = 2; PhaseStart = FPlatformTime::Seconds(); return false;
        }
        Car->ApplyDriveInput(0,1,0,true);
        if (Car->GetVelocity().Size() > 30 && Elapsed < 10) return false;
        Test->TestTrue(TEXT("Exit after braking"), Car->TryExit());
        Test->TestTrue(TEXT("Character regains control"), PC->GetPawn() == Character.Get());
        for (int32 Cycle=0; Cycle<20; ++Cycle)
        {
            if (!Test->TestTrue(TEXT("PIE enter"),Car->TryEnter(Character.Get()))) break;
            if (!Test->TestTrue(TEXT("PIE exit"),Car->TryExit())) break;
        }
        if (Car->TryEnter(Character.Get()))
        {
            Car->Destroy();
            Test->TestTrue(TEXT("Destroyed car restores control"), PC->GetPawn() == Character.Get());
            Test->TestFalse(TEXT("Destroyed car restores visibility"), Character->IsHidden());
        }
        return true;
    }
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKalexDrivingTest, "Kalex.Gameplay.DrivingPIE",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FKalexDrivingTest::RunTest(const FString& Parameters)
{
    if (!FEditorFileUtils::LoadMap(TEXT("/Game/KalexUrban/Levels/L_GameplayFoundation"), false, true)) return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    ADD_LATENT_AUTOMATION_COMMAND(FKalexDrivingCommand(this));
    ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand());
    return true;
}
#endif

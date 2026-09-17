#include "Procedural/LimaDistrictGenerator.h"
#include "Procedural/LimaRoadSpline.h"
#include "Data/LimaDistrictStyle.h"

ALimaDistrictGenerator::ALimaDistrictGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ALimaDistrictGenerator::GenerateDistrict()
{
    for (int32 Index = 0; Index < Roads.Num(); ++Index)
    {
        ALimaRoadSpline* Road = Roads[Index];

        if (!IsValid(Road))
        {
            continue;
        }

        Road->Seed = Seed + Index * 7919;
        Road->DistrictStyle = DistrictStyle;

        Road->GenerateStreet();
    }
}

void ALimaDistrictGenerator::ClearDistrict()
{
    for (ALimaRoadSpline* Road : Roads)
    {
        if (IsValid(Road))
        {
            Road->ClearGenerated();
        }
    }
}
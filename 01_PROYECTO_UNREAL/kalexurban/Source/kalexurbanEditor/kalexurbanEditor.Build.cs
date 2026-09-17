using UnrealBuildTool;
public class kalexurbanEditor : ModuleRules
{
    public kalexurbanEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "UnrealEd", "kalexurban", "ChaosVehicles", "InputCore" });
    }
}

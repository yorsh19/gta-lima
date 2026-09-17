using UnrealBuildTool;

public class kalexurban : ModuleRules
{
    public kalexurban(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EnhancedInput",
                "Json",
                "JsonUtilities",
                "Landscape",
                "ChaosVehicles",
                "PhysicsCore",
                "UMG",
                "Slate",
                "SlateCore",
                "Niagara"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}

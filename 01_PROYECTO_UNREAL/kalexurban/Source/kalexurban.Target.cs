using UnrealBuildTool;
using System.Collections.Generic;

public class kalexurbanTarget : TargetRules
{
    public kalexurbanTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;

        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

        ExtraModuleNames.Add("kalexurban");
    }
}
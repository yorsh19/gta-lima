using UnrealBuildTool;
using System.Collections.Generic;

public class kalexurbanEditorTarget : TargetRules
{
    public kalexurbanEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

        ExtraModuleNames.Add("kalexurban");
        ExtraModuleNames.Add("kalexurbanEditor");
    }
}

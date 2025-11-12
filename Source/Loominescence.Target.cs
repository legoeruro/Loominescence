using UnrealBuildTool;
using System.Collections.Generic;

public class LoominescenceTarget : TargetRules
{
    public LoominescenceTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("Loominescence");
    }
}

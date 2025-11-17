using UnrealBuildTool;
using System.Collections.Generic;

public class LoominescenceEditorTarget : TargetRules
{
    public LoominescenceEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("Loominescence");
    }
}

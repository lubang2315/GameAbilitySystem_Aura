// 由来时路褒贬不一制作

using UnrealBuildTool;
using System.Collections.Generic;

public class Aura_EndEditorTarget : TargetRules
{
	public Aura_EndEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "Aura_End" } );
	}
}

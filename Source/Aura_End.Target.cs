// 由来时路褒贬不一制作

using UnrealBuildTool;
using System.Collections.Generic;

public class Aura_EndTarget : TargetRules
{
	public Aura_EndTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "Aura_End" } );
	}
}

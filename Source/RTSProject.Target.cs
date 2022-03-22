using UnrealBuildTool;
using System.Collections.Generic;

public class RTSProjectTarget : TargetRules
{
	public RTSProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "RTSProject", "MeshProcessingPlugin" } );
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RTSProject : ModuleRules
{
	public RTSProject(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG" });

		PublicDependencyModuleNames.AddRange(new string[] { "AIModule", "NavigationSystem" });
		PublicDependencyModuleNames.AddRange(new string[] { "RHI", "RenderCore" });
		PublicDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

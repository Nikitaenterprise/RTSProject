// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RTSProject : ModuleRules
{
	public RTSProject(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        MinFilesUsingPrecompiledHeaderOverride = 1;
       
        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core", 
	        "CoreUObject", 
	        "Engine", 
	        "InputCore", 
	        "UMG", 
	        "Slate", 
	        "SlateCore", 
	        "AIModule", 
	        "NavigationSystem", 
	        "GameplayAbilities",
	        "RHI", 
	        "RenderCore",
			"EnhancedInput"
        });
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"Slate",
			"SlateCore",
            "EnhancedInput",
			"CommonUI"
        });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

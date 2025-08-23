// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EnemyLockOnTargeting : ModuleRules
{
	public EnemyLockOnTargeting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Added "GameplayTags", "Paper2D", "AIModule", "NavigationSystem"
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", 
			"GameplayTags", "Paper2D", "AIModule", "NavigationSystem" });

        PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

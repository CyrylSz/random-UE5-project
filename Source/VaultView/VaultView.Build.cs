// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VaultView : ModuleRules
{
	public VaultView(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"VaultView",
			"VaultView/Variant_Platforming",
			"VaultView/Variant_Platforming/Animation",
			"VaultView/Variant_Combat",
			"VaultView/Variant_Combat/AI",
			"VaultView/Variant_Combat/Animation",
			"VaultView/Variant_Combat/Gameplay",
			"VaultView/Variant_Combat/Interfaces",
			"VaultView/Variant_Combat/UI",
			"VaultView/Variant_SideScrolling",
			"VaultView/Variant_SideScrolling/AI",
			"VaultView/Variant_SideScrolling/Gameplay",
			"VaultView/Variant_SideScrolling/Interfaces",
			"VaultView/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

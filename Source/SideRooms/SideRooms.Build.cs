// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SideRooms : ModuleRules
{
	public SideRooms(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}

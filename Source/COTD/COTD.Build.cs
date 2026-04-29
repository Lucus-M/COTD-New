// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class COTD : ModuleRules
{
	public COTD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "AIModule", "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}

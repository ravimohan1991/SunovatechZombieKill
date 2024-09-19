// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SunovatechZombieKill : ModuleRules
{
	public SunovatechZombieKill(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ChaosVehicles", "PhysicsCore", "AIModule" });
	}
}

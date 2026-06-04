// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SeaProject : ModuleRules
{
	public SeaProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"HeadMountedDisplay",
			"XRBase"
		});

        PrivateDependencyModuleNames.AddRange(new string[] {  });

		// 모듈 루트를 인클루드 경로에 추가 → "Scan/...", "Player/..." 처럼
		// 하위 폴더를 경로로 #include 할 수 있게 함
		PrivateIncludePaths.Add(ModuleDirectory);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

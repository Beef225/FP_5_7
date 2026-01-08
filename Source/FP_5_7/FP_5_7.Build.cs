// FP_5_7.Build.cs
using UnrealBuildTool;

public class FP_5_7 : ModuleRules
{
	public FP_5_7(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"InputCore",
			"EnhancedInput"
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new[]
			{
				"UnrealEd",
				"AssetRegistry",
				"AssetTools",
				"Json",
				"JsonUtilities",
				"DesktopPlatform",
				"Slate",
				"SlateCore"
			});
		}
	
	}
}

namespace UnrealBuildTool.Rules
{
	public class PCG_CustomNodes : ModuleRules
	{
		public PCG_CustomNodes(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(
				new string[] {
					"Core",
					"CoreUObject",
					"Engine",
					"PCG"
				});
		}
	}
}

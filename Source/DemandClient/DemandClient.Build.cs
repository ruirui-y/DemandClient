// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class DemandClient : ModuleRules
{
    public DemandClient(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Sockets",
            "Networking",
            "Json"
            });

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string APL = Path.Combine(ModuleDirectory, "Android", "DemandClient_APL.xml");
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", APL);
        }
    }
}

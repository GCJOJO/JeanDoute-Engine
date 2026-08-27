project "CSharp-Core"
	language "C#"
	kind "SharedLib"
	dotnetframework "net9.0"
	clr "Unsafe"

	targetdir ("%{prj.location}/bin/" .. csharp_outputdir)
	objdir ("%{prj.location}/bin-int/" .. csharp_outputdir)

	links
	{
		"Coral.Managed"
	}

	vsprops {
		AppendTargetFrameworkToOutputPath = "false",
		Nullable = "enable",
		CopyLocalLockFileAssemblies = "true",
		EnableDynamicLoading = "true",
	}

	files
	{
		"Source/**.cs",
		"Properties/**.cs"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"
	
	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"
include "../vendor/premake/premake_customization/debugger_type_extension.lua"

project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	--debuggertype "NativeWithManagedCore"
	--dependson "GamingEngine-ScriptCore"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Sources/**.h",
		"Sources/**.cpp"
	}

	includedirs
	{
		"Sources",
		"%{wks.location}/Engine/Sources",
		"%{wks.location}/Engine/vendor/spdlog/include",
		"%{wks.location}/Engine/vendor",
		"%{IncludeDir.bgfx}",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.yaml_cpp}",
	}

	links 
	{
		"Engine",
		"ImGui",
		"yaml-cpp"
	}

	postbuildcommands { 
		"{COPYDIR} %[%{prj.location}/assets] %[%{cfg.buildtarget.directory}/assets]",
		"{COPYDIR} %[%{prj.location}/Binaries] %[%{cfg.buildtarget.directory}/Binaries]",
		"{COPYFILE} %[%{prj.location}/imgui.ini] %[%{cfg.buildtarget.directory}/imgui.ini]",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ENGINE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ENGINE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ENGINE_DIST"
		runtime "Release"
		optimize "on"
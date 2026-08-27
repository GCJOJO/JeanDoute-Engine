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
		"%{wks.location}/%{prj.name}/Sources",
		"%{wks.location}/Engine/Includes",
		"%{wks.location}/Engine/vendor/spdlog/include",
		"%{wks.location}/Engine/vendor",
		"%{IncludeDir.bgfx}",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}",
		"%{IncludeDir.yaml_cpp}",
	}

	links 
	{
		"Engine",
		"ImGui",
		"yaml-cpp"
	}

	defines{
		"BX_CONFIG_DEBUG"
	}

	postbuildcommands { 
		"{COPYDIR} %[%{wks.location}/Engine/Shaders] %[%{prj.location}/Shaders]",
		--"{COPYDIR} %[%{prj.location}/Binaries] %[%{cfg.buildtarget.directory}/Binaries]",
		--"{COPYFILE} %[%{prj.location}/imgui.ini] %[%{cfg.buildtarget.directory}/imgui.ini]",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ENGINE_PLATFORM_WINDOWS"
		}

		buildoptions
		{
			"/utf-8",
			"/Zc:__cplusplus",
			"/Zc:preprocessor"
		}

		postbuildcommands { 
			"{COPYFILE} %[%{wks.location}/vendor/shaderc.exe] %[%{cfg.buildtarget.directory}/shaderc.exe]",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

		defines
		{
			"ENGINE_DEBUG",
			"BX_CONFIG_DEBUG"
		}

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ENGINE_DIST"
		runtime "Release"
		optimize "on"
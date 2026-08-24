project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	characterset "Unicode"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "enginepch.h"
	pchsource "Sources/enginepch.cpp"

	files
	{
		"Includes/**.h",
		"Sources/**.cpp",
		"Shaders/**",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/ImGuizmo/ImGuizmo.h",     
		"vendor/ImGuizmo/ImGuizmo.cpp"     
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs 
	{
		"Includes",
		"vendor/spdlog/include",
		"%{IncludeDir.bgfx}",
		"%{IncludeDir.bimg}",
		"%{IncludeDir.bx}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.yaml_cpp}",
	}

	links 
	{
		"bgfx",
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		
		"opengl32.lib",
		"%{LibraryDir.VulkanSDK}/vulkan-1.lib",
	}

	filter "files:vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GAMING_ENGINE",
			"ENGINE_PLATFORM_WINDOWS",
			"ENGINE_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			"CURL_STATICLIB",
		}

		--[[links
		{
			"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.BCrypt}",
		}]]--

		buildoptions
		{
			"/utf-8",
			"/Zc:__cplusplus",
			"/Zc:preprocessor"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

		defines
		{
			"ENGINE_DEBUG",
			"BX_CONFIG_DEBUG"
		}

		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

	filter "configurations:Dist"
		defines "ENGINE_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

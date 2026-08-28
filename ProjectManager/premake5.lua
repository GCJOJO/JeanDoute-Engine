project "ProjectManager"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/Editor")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Sources/**.h",
		"Sources/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Engine/Includes",
		"%{wks.location}/Engine/vendor/spdlog/include",
		"%{wks.location}/Engine/vendor",
        "%{IncludeDir.bgfx}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.minizip}",
		"%{IncludeDir.libcurl}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.zlib}",
	}

	links 
	{
		"Engine",
		"ImGui",
		--"yaml-cpp",
	}

	postbuildcommands { 
			--"{COPYFILE} %[%{DynamicLibrary.libcurl}] %[%{cfg.buildtarget.directory}libcurl-x64.dll]",
			"{COPYFILE} %[%{prj.location}curl-ca-bundle.crt] %[%{cfg.buildtarget.directory}curl-ca-bundle.crt]",
			--"{COPYFILE} %[%{prj.location}/imgui.ini] %[%{cfg.buildtarget.directory}/imgui.ini]",
			--"{COPYDIR} %[%{prj.location}/assets] %[%{cfg.buildtarget.directory}/assets]",
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
include "vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "JeanDoute-Engine"
	startproject "Editor"
	multiprocessorcompile "On"

	filter { "language:C++" }
  		architecture "x64"
	filter { "language:C" }
  		architecture "x64"
	filter {}

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"



group "Dependencies"
	include "Engine/vendor/bgfx"
	include "Engine/vendor/bimg"
	include "Engine/vendor/bx"
	include "Engine/vendor/GLFW"
	include "Engine/vendor/Glad"
	include "Engine/vendor/imgui"
	include "Engine/vendor/yaml-cpp"
group ""

group "Core"
	include "Engine"
	--include "Engine-ScriptCore"
	--include "Runtime"
group ""

-- group "Misc"
-- 	include "Sandbox"
-- group "" 

group "Tools"
	include "Editor"
	--include "ProjectManager"
group ""
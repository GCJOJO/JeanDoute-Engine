include "vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

local BX_DIR   = path.getabsolute("Engine/vendor/bx")
local BGFX_DIR = path.getabsolute("Engine/vendor/bgfx")
local vswhere = path.getabsolute("vendor/vswhere.exe")

local function genieExePath()
	if os.target() == "windows" then
		return path.join(BX_DIR, "tools/bin/windows/genie.exe")
	elseif os.target() == "macosx" then
		return path.join(BX_DIR, "tools/bin/darwin/genie")
	else 
		return path.join(BX_DIR, "tools/bin/linux/genie")
	end
end

local function findmsbuild()

	if not os.isfile(vswhere) then
		error("vswhere.exe not found at " .. vswhere)
	end

	local cmd = '"' .. vswhere .. '" -latest -requires Microsoft.Component.MSBuild -find MSBuild/**/Bin/MSBuild.exe'
	local pipe = io.popen(cmd)
	local msbuildPath = pipe:read("*a")
	pipe:close()

	msbuildPath = msbuildPath:gsub("[\r\n]+", "") -- Remove whitespace
	if msbuildPath == "" then
		error("MSBuild.exe not found. Please ensure Visual Studio is installed with the MSBuild component.")
		return nil
	end
	print("Found MSBuild at: " .. msbuildPath)
	return msbuildPath
end

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
	csharp_outputdir = "%{cfg.buildcfg}-%{cfg.system}"

	newaction {
		trigger 	= "build-shaderc",
		description = "Builds shaderc tool",

		execute = function()
			local genie = genieExePath()
			local cwd = os.getcwd()

			print("Generating shaderc project files...")
			os.chdir(BGFX_DIR)
			
			print("Genie path : " .. genie)
			print("BGFX_DIR : " .. BGFX_DIR)

			local ok, errCode
			if os.target() == "windows" then
				ok, errCode = os.execute(string.format('"%s" --with-tools vs2022', genie))
			elseif os.target() == "macosx" then
				ok, errCode = os.execute(string.format('"%s" --with-tools xcode4', genie))
			else 
				ok, errCode = os.execute(string.format('"%s" --with-tools gmake', genie))
			end

			if not ok then
				os.chdir(cwd)
				error("Error generating shaderc project files: " .. tostring(errCode))
			end

			print("Building shaderc tool...")
			local output_dir = path.join(BGFX_DIR, ".build")
			local shaderc_exe
			local shaderc_output_exe

			if os.target() == "windows" then
				local msbuildPath = findmsbuild()
				local projectDir = path.join(BGFX_DIR, ".build/projects/vs2022/shaderc.vcxproj")
				shaderc_exe = "shadercRelease.exe"
				shaderc_output_exe = "shaderc.exe"
				output_dir = path.join(output_dir, "win64_vs2022", "bin", shaderc_exe)
				

				print("shaderc project path : " .. projectDir)
				print("shaderc output path : " .. output_dir)

				ok, errCode = os.execute(string.format('"%s" /m %s /p:Configuration=Release /p:Platform=x64', msbuildPath, projectDir))
			elseif os.target() == "macosx" then
				ok, errCode = os.execute('xcodebuild -project tools/shaderc/shaderc.xcodeproj -configuration Release')
			else 
				ok, errCode = os.execute('make -C tools/shaderc config=release')
			end
			
			os.chdir(cwd)

			if not ok then
				error("Error building shaderc tool: " .. tostring(errCode))
			end

			print("Copying output file" .. output_dir .. " to vendor/" .. shaderc_output_exe)
			os.copyfile(output_dir, path.join(cwd, "vendor", shaderc_output_exe))
		end
	}

group "Dependencies"
	include "Engine/vendor/Assimp"
	include "Engine/vendor/bgfx"
	include "Engine/vendor/bimg"
	include "Engine/vendor/bx"
	include "Engine/vendor/coral/coral.native"
	include "Engine/vendor/coral/coral.managed"
	include "Engine/vendor/GLFW"
	include "Engine/vendor/Glad"
	include "Engine/vendor/imgui"
	include "Engine/vendor/msdf-atlas-gen"
	include "Engine/vendor/yaml-cpp"
group ""

group "Core"
	include "Engine"
	include "CSharp-Core"
	--include "Runtime"
group ""

-- group "Misc"
-- 	include "Sandbox"
-- group "" 

group "Tools"
	include "Editor"
	--include "ProjectManager"
group ""
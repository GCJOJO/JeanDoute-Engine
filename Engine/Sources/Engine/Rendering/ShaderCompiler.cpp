#include "enginepch.h"
#include "Engine/Rendering/ShaderCompiler.h"
#include "Engine/Utils/PlatformUtils.h"

namespace Engine 
{
	static std::filesystem::path GetShadercExecutable()
	{
		return OS::GetExecutableDir() / "shaderc.exe";
	}

	bool CompileShader(const std::filesystem::path& shaderFile)
	{
		bgfx::RendererType::Enum renderer = bgfx::getRendererType();
		
		ShaderCompiler::CommandArguments args = ShaderCompiler::GetArgumentsForRenderer(renderer);
		ShaderCompiler::ShaderType type = ShaderCompiler::ShaderTypeFromExtension(shaderFile);

		std::string profile = 
			(type == ShaderCompiler::ShaderType::Vertex) ? args.profileVs :
			(type == ShaderCompiler::ShaderType::Fragment) ? args.profileFs:
			"cs_5_0"; // Compute

		std::filesystem::path shadercExecutable = GetShadercExecutable();
		std::filesystem::path outPath = shaderFile.parent_path() / profile / shaderFile.filename();
		const std::filesystem::path binExtension{ ".bin" };
		outPath.replace_extension(binExtension);

		std::filesystem::path varyingPath = shaderFile.parent_path() / "varying.def.sc";

		try {
			std::filesystem::create_directories(outPath.parent_path());

			std::string cmd = std::format(R"("{}" -f "{}" -o "{}" --type {} --platform {} --varyingdef {} -p {} -O 3)", 
				shadercExecutable.string(),
				shaderFile.string(),
				outPath.string(),
				ShaderCompiler::ShaderTypeString(type),
				args.platform,
				varyingPath.string(),
				profile);
			
			std::filesystem::path commonFolder = OS::GetCurrentDir() / "Shaders" / "Common";

			cmd += std::format(R"( -i {})", commonFolder.string());

			ENGINE_CORE_WARN("Compiling Shader... Command : \n {} \n", cmd);

			int exitCode = Process::StartProcess(cmd.c_str());
			return exitCode == 0;
		}
		catch (std::exception& e)
		{
			ENGINE_CORE_ERROR("Couldn't parse shaderc command !");
			ENGINE_CORE_ERROR("--------------------------------");
			ENGINE_CORE_ERROR("shaderc executable : " + shadercExecutable.string());
			ENGINE_CORE_ERROR("shader input file : " + shaderFile.string());
			ENGINE_CORE_ERROR("shader output file : " + outPath.string());
			ENGINE_CORE_ERROR("shader type : " + ShaderCompiler::ShaderTypeString(type));
			ENGINE_CORE_ERROR("platform : " + args.platform);
			ENGINE_CORE_ERROR("varying path : " + varyingPath.string());
			ENGINE_CORE_ERROR("--------------------------------");
			return false;
		}
	}

	bool ShaderCompiler::CompileShaders(const std::filesystem::path& shaderDirectory)
	{
		std::filesystem::path absoluteShaderDirectory = OS::GetCurrentDir() / shaderDirectory;

		if (!std::filesystem::is_directory(absoluteShaderDirectory))
			return false;

		for (auto& file : std::filesystem::directory_iterator(absoluteShaderDirectory))
		{
			if (file.is_directory())
				continue;

			if(file.path().extension() == ".sc")
				continue;

			CompileShader(file);
		}

		return true;
	}

}
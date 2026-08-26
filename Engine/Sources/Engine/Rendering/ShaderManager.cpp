#include "enginepch.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Utils/PlatformUtils.h"

#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bx/readerwriter.h"

#include <array>
#include <ranges>

namespace Engine 
{
	std::filesystem::path ShaderManager::GetShadercExecutable()
	{
		return OS::GetExecutableDir() / "shaderc.exe";
	}

	bool ShaderManager::CompileShader(const std::filesystem::path& shaderFile)
	{
		bgfx::RendererType::Enum renderer = bgfx::getRendererType();
		
		ShaderManager::CommandArguments args = ShaderManager::GetArgumentsForRenderer(renderer);
		Shader::Type type = ShaderManager::ShaderTypeFromExtension(shaderFile);

		std::string profile = 
			(type == Shader::Type::Vertex) ? args.profileVs :
			(type == Shader::Type::Fragment) ? args.profileFs:
			"cs_5_0"; // Compute

		std::filesystem::path shadercExecutable = GetShadercExecutable();
		std::filesystem::path outPath = shaderFile.parent_path() / profile / shaderFile.filename();
		const std::filesystem::path binExtension{ ".bin" };
		outPath.replace_extension(binExtension);

		std::filesystem::path varyingPath = shaderFile.parent_path() / "varying.def.sc";

		try {
			std::filesystem::create_directories(outPath.parent_path());

			std::string cmd = std::format(R"("{}" -f "{}" -o "{}" --type {} --platform {} --varying {} -p {} -O 3)", 
				shadercExecutable.string(),
				shaderFile.string(),
				outPath.string(),
				ShaderManager::ShaderTypeString(type),
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
			ENGINE_CORE_ERROR("shader type : " + ShaderManager::ShaderTypeString(type));
			ENGINE_CORE_ERROR("platform : " + args.platform);
			ENGINE_CORE_ERROR("varying path : " + varyingPath.string());
			ENGINE_CORE_ERROR("--------------------------------");
			return false;
		}
	}

	bool ShaderManager::CompileShaders(const std::filesystem::path& shaderDirectory)
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

	bgfx::ShaderHandle ShaderManager::LoadShader(const std::string& shaderName)
	{
		// Use std::filesystem instead
		std::filesystem::path filePath("shaders");

		bgfx::RendererType::Enum rdType = bgfx::getRendererType();

		switch (bgfx::getRendererType())
		{
		case bgfx::RendererType::Noop:
		case bgfx::RendererType::Direct3D11: filePath /= "dxbc";  break;
		case bgfx::RendererType::Direct3D12: filePath /= "dxil";  break;
		case bgfx::RendererType::Agc:
		case bgfx::RendererType::Gnm:        filePath /= "pssl";  break;
		case bgfx::RendererType::Metal:      filePath /= "metal"; break;
		case bgfx::RendererType::Nvn:        filePath /= "nvn";   break;
		case bgfx::RendererType::OpenGL:     filePath /= "glsl";  break;
		case bgfx::RendererType::OpenGLES:   filePath /= "essl";  break;
		case bgfx::RendererType::Vulkan:     filePath /= "spirv"; break;
		case bgfx::RendererType::WebGPU:     filePath /= "wgsl";  break;

		case bgfx::RendererType::Count:
			ENGINE_CORE_ASSERT(false, "Invalid Renderer Type");
			break;
		}

		std::string fileName = shaderName + ".bin";
		filePath /= fileName;

		char buffer[2048];

		// Open file
		std::ifstream shaderFile(filePath, std::ios::binary); // and since you want bytes rather than
		if (!shaderFile.is_open())
		{
			ENGINE_CORE_ERROR("Unable to open file", filePath.string());
			return BGFX_INVALID_HANDLE;
		}
		// characters, strongly consider opening the
		// File in binary mode with std::ios_base::binary
		// Get length of file
		shaderFile.seekg(0, std::ios::end);
		size_t shader_filesize = shaderFile.tellg();
		shaderFile.seekg(0, std::ios::beg);

		const bgfx::Memory* memory = bgfx::alloc(shader_filesize + 1);

		bx::StaticMemoryBlockWriter memWriter(memory->data, memory->size);

		// Don't overflow the buffer!
		if (shader_filesize > sizeof(buffer)) {
			shader_filesize = sizeof(buffer);
		}

		// Read file
		shaderFile.read(buffer, shader_filesize);
		shaderFile.close();

		bx::Error error;
		memWriter.write(buffer, shader_filesize, &error);
		ENGINE_CORE_ASSERT(error.isOk());

		bgfx::ShaderHandle handle = bgfx::createShader(memory);
		bgfx::setName(handle, shaderName.data(), shaderName.length());

		return handle;
	}

	bgfx::ProgramHandle ShaderManager::LoadProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName)
	{
		bgfx::ShaderHandle vsh = LoadShader(vertexShaderName);
		bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
		if (!fragmentShaderName.empty())
			fsh = LoadShader(fragmentShaderName);
		
		return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
	}

}
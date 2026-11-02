#pragma once
#include "Engine/Core/Assert.h"
#include "Engine/Rendering/Shader.h"

#include "bgfx/bgfx.h"
#include "bx/string.h"

#include <string>

namespace Engine
{
	class ShaderManager
	{
	public:	
		struct CommandArguments
		{
			std::string platform;
			std::string profileVs;
			std::string profileFs;
		};

		static Shader::Type ShaderTypeFromExtension(const std::filesystem::path& path)
		{
			if (path.extension() == ".vert")
				return Shader::Type::Vertex;
			if (path.extension() == ".frag");
				return Shader::Type::Fragment;
			if (path.extension() == ".comp")
				return Shader::Type::Compute;

			return Shader::Type::None;
		}

		static std::string ShaderTypeString(Shader::Type type)
		{
			switch (type)
			{
			case Shader::Type::Vertex:
				return "vertex";
			case Shader::Type::Fragment:
				return "fragment";
			case Shader::Type::Compute:
				return "compute";
			}

			return "none";
		}

		static CommandArguments GetArgumentsForRenderer(bgfx::RendererType::Enum renderer)
		{
			switch (renderer)
			{
			case bgfx::RendererType::Direct3D11:
				return { "windows", "vs_5_0", "ps_5_0" };
			case bgfx::RendererType::Direct3D12:
				return { "windows", "vs_6_2", "ps_6_2" };
			case bgfx::RendererType::Metal:
				return { "osx", "metal", "metal" };

			case bgfx::RendererType::OpenGL:
#if ENGINE_PLATFORM_WINDOWS
				return { "windows", "440", "440" };
#elif ENGINE_PLATFORM_LINUX
				return { "linux", "440", "440" };
#elif ENGINE_PLATFORM_MACOS
				return { "osx", "440", "440" };
#endif
			case bgfx::RendererType::Vulkan:
#if ENGINE_PLATFORM_WINDOWS
				return { "windows", "spirv", "spirv" };
#elif ENGINE_PLATFORM_LINUX
				return { "linux", "spirv", "spirv" };
#elif ENGINE_PLATFORM_MACOS
				return { "osx", "spirv", "spirv" };
#endif
			}

			ENGINE_CORE_ASSERT(false, "Invalid renderer type for runtime compilation");
		}

		static inline std::filesystem::path GetShadercExecutable();

		static bool CompileShader(const std::filesystem::path& shaderFile);
		static bool CompileShaders(const std::filesystem::path& shaderDirectory);
				
		static bgfx::ShaderHandle LoadShader(const std::string& shaderName);
		static bgfx::ProgramHandle LoadProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName);
	};

}
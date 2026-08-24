#pragma once
#include "Engine/Core/Assert.h"

#include <string>
#include <bgfx/bgfx.h>

namespace Engine
{
	class ShaderCompiler
	{
	public:
		enum ShaderType
		{
			None = 0,
			Vertex,
			Fragment,
			Compute,
		};
		
		struct CommandArguments
		{
			std::string platform;
			std::string profileVs;
			std::string profileFs;
		};

		static ShaderType ShaderTypeFromExtension(const std::filesystem::path& path)
		{
			if (path.extension() == ".vert")
				return ShaderType::Vertex;
			if (path.extension() == ".frag");
				return ShaderType::Fragment;
			if (path.extension() == ".comp")
				return ShaderType::Compute;

			return ShaderType::None;
		}

		static std::string ShaderTypeString(ShaderType type)
		{
			switch (type)
			{
			case Vertex:
				return "vertex";
			case Fragment:
				return "fragment";
			case Compute:
				return "compute";
			}

			return "none";
		}

		static CommandArguments GetArgumentsForRenderer(bgfx::RendererType::Enum renderer)
		{
			switch (renderer)
			{
			case bgfx::RendererType::Direct3D11:
			case bgfx::RendererType::Direct3D12:
				return { "windows", "vs_5_0", "ps_5_0" };
			case bgfx::RendererType::Metal:
				return { "osx", "metal", "metal" };

			case bgfx::RendererType::OpenGL:
#if ENGINE_PLATFORM_WINDOWS
				return { "windows", "440", "440" };
#elif ENGINE_PLATFORM_LINUX
				return { "linux", "440", "440" };
#endif
			case bgfx::RendererType::Vulkan:
#if ENGINE_PLATFORM_WINDOWS
				return { "windows", "spirv", "spirv" };
#elif ENGINE_PLATFORM_LINUX
				return { "linux", "spirv", "spirv" };
#endif
			}

			ENGINE_CORE_ASSERT(false, "Invalid renderer type for runtime compilation");
		}

		static bool CompileShaders(const std::filesystem::path& shaderDirectory);
	};

}
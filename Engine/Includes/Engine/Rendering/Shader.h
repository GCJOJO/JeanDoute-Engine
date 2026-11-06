#pragma once
#include "Engine/Assets/Asset.h"
#include "Engine/Core/UUID.h"

#include <bgfx/bgfx.h>

namespace Engine
{
	class Shader : public Asset
	{
	public:
		enum Type
		{
			None = 0,
			Vertex,
			Fragment,
			Compute,
		};

		Shader(Type shaderType, std::filesystem::path& shaderFile);
		~Shader();
				
		bgfx::ShaderHandle& GetShaderHandle() { return m_ShaderHandle; }
		operator bgfx::ShaderHandle() const { return m_ShaderHandle; }

		Asset::Type GetType() const override;
	private:
		Type m_ShaderType;
		bgfx::ShaderHandle m_ShaderHandle;
	};

	class ShaderProgram
	{
	public:
		ShaderProgram(SharedPtr<Shader> vertexShader, SharedPtr<Shader> fragmentShader);
		~ShaderProgram();
		
		bgfx::ProgramHandle& GetProgramHandle() { return m_ProgramHandle; }

	private:
		SharedPtr<Shader> m_VertexShader;
		SharedPtr<Shader> m_FragmentShader;

		bgfx::ProgramHandle m_ProgramHandle;
	public:
	};
}
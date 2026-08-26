#include "enginepch.h"
#include "Engine/Rendering/Shader.h"
#include "Engine/Rendering/ShaderManager.h"

namespace Engine
{
	Shader::Shader(Type shaderType, std::filesystem::path& shaderFile)
	{
		m_ShaderType = shaderType;
		std::filesystem::path shaderFileName = shaderFile.filename();
		shaderFileName.replace_extension();

		m_ShaderHandle = ShaderManager::LoadShader(shaderFileName.string());
	}

	Shader::~Shader()
	{
		if(bgfx::isValid(m_ShaderHandle))
		{
			bgfx::destroy(m_ShaderHandle);
			m_ShaderHandle = BGFX_INVALID_HANDLE;
		}
	}

	Asset::Type Shader::GetType() const
	{
		return Asset::Type::Shader;
	}

	ShaderProgram::ShaderProgram(Ref<Shader> vertexShader, Ref<Shader> fragmentShader)
		: m_VertexShader(vertexShader), m_FragmentShader(fragmentShader)
	{
		m_ProgramHandle = bgfx::createProgram(m_VertexShader->GetShaderHandle(), m_FragmentShader->GetShaderHandle(), false);
	}

	ShaderProgram::~ShaderProgram()
	{
		if(bgfx::isValid(m_ProgramHandle))
		{
			bgfx::destroy(m_ProgramHandle);
			m_ProgramHandle = BGFX_INVALID_HANDLE;
		}

		m_VertexShader = nullptr;
		m_FragmentShader = nullptr;
	}
}